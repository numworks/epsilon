#include "apps_container.h"
#include "global_preferences.h"
#include <ion.h>
#include <poincare/init.h>
#include <poincare/exception_checkpoint.h>

extern "C" {
#include <assert.h>
}

using namespace Shared;

#if EPSILON_BOOT_PROMPT == EPSILON_BETA_PROMPT

static I18n::Message sPromptMessages[] = {
  I18n::Message::BetaVersion,
  I18n::Message::BetaVersionMessage1,
  I18n::Message::BetaVersionMessage2,
  I18n::Message::BetaVersionMessage3,
  I18n::Message::BlankMessage,
  I18n::Message::BetaVersionMessage4,
  I18n::Message::BetaVersionMessage5,
  I18n::Message::BetaVersionMessage6};

static KDColor sPromptColors[] = {
  KDColorBlack,
  KDColorBlack,
  KDColorBlack,
  KDColorBlack,
  KDColorWhite,
  KDColorBlack,
  KDColorBlack,
  Palette::YellowDark};

#elif EPSILON_BOOT_PROMPT == EPSILON_UPDATE_PROMPT

static I18n::Message sPromptMessages[] = {
  I18n::Message::UpdateAvailable,
  I18n::Message::UpdateMessage1,
  I18n::Message::UpdateMessage2,
  I18n::Message::BlankMessage,
  I18n::Message::UpdateMessage3,
  I18n::Message::UpdateMessage4};

static KDColor sPromptColors[] = {
  KDColorBlack,
  KDColorBlack,
  KDColorBlack,
  KDColorWhite,
  KDColorBlack,
  Palette::YellowDark};

#endif

AppsContainer::AppsContainer() :
  Container(),
  m_window(),
  m_emptyBatteryWindow(),
  m_globalContext(),
  m_variableBoxController(),
  m_examPopUpController(this),
#if EPSILON_BOOT_PROMPT == EPSILON_BETA_PROMPT
  m_promptController(sPromptMessages, sPromptColors, 8),
#elif EPSILON_BOOT_PROMPT == EPSILON_UPDATE_PROMPT
  m_promptController(sPromptMessages, sPromptColors, 6),
#endif
  m_batteryTimer(BatteryTimer(this)),
  m_suspendTimer(SuspendTimer(this)),
  m_backlightDimmingTimer(),
  m_homeSnapshot(),
  m_onBoardingSnapshot(),
  m_hardwareTestSnapshot(),
  m_usbConnectedSnapshot()
{
  m_emptyBatteryWindow.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
#if __EMSCRIPTEN__
  /* AppsContainer::poincareCircuitBreaker uses Ion::Keyboard::scan(), which
   * calls emscripten_sleep. If we set the poincare circuit breaker, we would
   * need to whitelist all the methods that might be in the call stack when
   * poincareCircuitBreaker is run. This means either whitelisting all Epsilon
   * (which makes bigger files to download and slower execution), or
   * whitelisting all the symbols (that's a big amount of symbols to find and
   * quite painy to maintain).
   * We just remove the circuit breaker for now.
   * TODO: Put the Poincare circuit breaker back on epsilon's web emulator */
#else
  Poincare::Expression::setCircuitBreaker(AppsContainer::poincareCircuitBreaker);
#endif
  Ion::Storage::sharedStorage()->setDelegate(this);
}

bool AppsContainer::poincareCircuitBreaker() {
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  return state.keyDown(Ion::Keyboard::Key::A6);
}

App::Snapshot * AppsContainer::hardwareTestAppSnapshot() {
  return &m_hardwareTestSnapshot;
}

App::Snapshot * AppsContainer::onBoardingAppSnapshot() {
  return &m_onBoardingSnapshot;
}

App::Snapshot * AppsContainer::usbConnectedAppSnapshot() {
  return &m_usbConnectedSnapshot;
}

void AppsContainer::reset() {
  // Empty storage (delete functions, variables, python scripts)
  Ion::Storage::sharedStorage()->destroyAllRecords();
  // Empty clipboard
  Clipboard::sharedClipboard()->reset();
  for (int i = 0; i < numberOfApps(); i++) {
    appSnapshotAtIndex(i)->reset();
  }
}

Poincare::Context * AppsContainer::globalContext() {
  return &m_globalContext;
}

MathToolbox * AppsContainer::mathToolbox() {
  return &m_mathToolbox;
}

VariableBoxController * AppsContainer::variableBoxController() {
  return &m_variableBoxController;
}

void AppsContainer::suspend(bool checkIfPowerKeyReleased) {
  resetShiftAlphaStatus();
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
#ifdef EPSILON_BOOT_PROMPT
  if (activeApp()->snapshot()!= onBoardingAppSnapshot() && activeApp()->snapshot() != hardwareTestAppSnapshot() && globalPreferences->showPopUp()) {
    activeApp()->displayModalViewController(&m_promptController, 0.f, 0.f);
  }
#endif
  Ion::Power::suspend(checkIfPowerKeyReleased);
  /* Ion::Power::suspend() completely shuts down the LCD controller. Therefore
   * the frame memory is lost. That's why we need to force a window redraw
   * upon wakeup, otherwise the screen is filled with noise. */
  Ion::Backlight::setBrightness(globalPreferences->brightnessLevel());
  m_backlightDimmingTimer.reset();
  window()->redraw(true);
}

bool AppsContainer::dispatchEvent(Ion::Events::Event event) {
  bool alphaLockWantsRedraw = updateAlphaLock();
  bool didProcessEvent = false;

  if (event == Ion::Events::USBEnumeration) {
    if (Ion::USB::isPlugged()) {
      App::Snapshot * activeSnapshot = (activeApp() == nullptr ? appSnapshotAtIndex(0) : activeApp()->snapshot());
      /* Just after a software update, the battery timer does not have time to
       * fire before the calculator enters DFU mode. As the DFU mode blocks the
       * event loop, we update the battery state "manually" here. */
      updateBatteryState();
      switchTo(usbConnectedAppSnapshot());
      Ion::USB::DFU();
      switchTo(activeSnapshot);
      didProcessEvent = true;
    } else {
      /* Sometimes, the device gets an ENUMDNE interrupts when being unplugged
       * from a non-USB communicating host (e.g. a USB charger). The interrupt
       * must me cleared: if not the next enumeration attempts will not be
       * detected. */
      Ion::USB::clearEnumerationInterrupt();
    }
  } else {
    didProcessEvent = Container::dispatchEvent(event);
  }

  if (!didProcessEvent) {
    didProcessEvent = processEvent(event);
  }
  if (event.isKeyboardEvent()) {
    m_backlightDimmingTimer.reset();
    m_suspendTimer.reset();
    Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
  }
  if (!didProcessEvent && alphaLockWantsRedraw) {
    window()->redraw();
    return true;
  }
  return didProcessEvent || alphaLockWantsRedraw;
}

bool AppsContainer::processEvent(Ion::Events::Event event) {
  if (event == Ion::Events::USBPlug) {
    if (Ion::USB::isPlugged()) {
      if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
        displayExamModePopUp(false);
      } else {
        Ion::USB::enable();
      }
      Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
    } else {
      Ion::USB::disable();
    }
    return true;
  }
  if (event == Ion::Events::Home || event == Ion::Events::Back) {
    switchTo(appSnapshotAtIndex(0));
    return true;
  }
  if (event == Ion::Events::OnOff) {
    suspend(true);
    return true;
  }
  return false;
}

void AppsContainer::switchTo(App::Snapshot * snapshot) {
  if (activeApp() && snapshot != activeApp()->snapshot()) {
    resetShiftAlphaStatus();
  }
  if (snapshot == hardwareTestAppSnapshot() || snapshot == onBoardingAppSnapshot()) {
    m_window.hideTitleBarView(true);
  } else {
    m_window.hideTitleBarView(false);
  }
  if (snapshot) {
    m_window.setTitle(snapshot->descriptor()->upperName());
  }
  Container::switchTo(snapshot);
}

void AppsContainer::run() {
  window()->setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  refreshPreferences();

  /* ExceptionCheckpoint stores the value of the stack pointer when setjump is
   * called. During a longjump, the stack pointer is set to this stored stack
   * pointer value, so the method where we call setjump must remain in the call
   * tree for the jump to work. */
  Poincare::ExceptionCheckpoint ecp;

  if (ExceptionRun(ecp)) {
    /* Normal execution. The exception checkpoint must be created before
     * switching to the first app, because the first app might create nodes on
     * the pool. */
#if EPSILON_ONBOARDING_APP
    switchTo(onBoardingAppSnapshot());
#else
    if (numberOfApps() == 2) {
      switchTo(appSnapshotAtIndex(1));
    } else {
      switchTo(appSnapshotAtIndex(0));
    }
#endif
  } else {
    // Exception
    if (activeApp() != nullptr) {
      activeApp()->snapshot()->reset();
    }
    switchTo(appSnapshotAtIndex(0));
    Poincare::Tidy();
    activeApp()->displayWarning(I18n::Message::PoolMemoryFull1, I18n::Message::PoolMemoryFull2, true);
  }
  Container::run();
  switchTo(nullptr);
}

bool AppsContainer::updateBatteryState() {
  bool batteryLevelUpdated = m_window.updateBatteryLevel();
  bool pluggedStateUpdated = m_window.updatePluggedState();
  bool chargingStateUpdated = m_window.updateIsChargingState();
  if (batteryLevelUpdated || pluggedStateUpdated || chargingStateUpdated) {
    return true;
  }
  return false;
}

void AppsContainer::refreshPreferences() {
  m_window.refreshPreferences();
}

void AppsContainer::displayExamModePopUp(bool activate) {
  m_examPopUpController.setActivatingExamMode(activate);
  activeApp()->displayModalViewController(&m_examPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
}

void AppsContainer::shutdownDueToLowBattery() {
  while (Ion::Battery::level() == Ion::Battery::Charge::EMPTY) {
    m_emptyBatteryWindow.redraw(true);
    Ion::Timing::msleep(3000);
    Ion::Power::suspend();
  }
  window()->redraw(true);
}

void AppsContainer::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus newStatus) {
  Ion::Events::setShiftAlphaStatus(newStatus);
  updateAlphaLock();
}

bool AppsContainer::updateAlphaLock() {
  return m_window.updateAlphaLock();
}

#ifdef EPSILON_BOOT_PROMPT
OnBoarding::PopUpController * AppsContainer::promptController() {
  return &m_promptController;
}
#endif

void AppsContainer::redrawWindow() {
  m_window.redraw();
}

void AppsContainer::examDeactivatingPopUpIsDismissed() {
  if (Ion::USB::isPlugged()) {
    Ion::USB::enable();
  }
}

void AppsContainer::storageDidChangeForRecord(const Ion::Storage::Record record) {
  if (activeApp()) {
    activeApp()->snapshot()->storageDidChangeForRecord(record);
  }
}

void AppsContainer::storageIsFull() {
  if (activeApp()) {
    activeApp()->displayWarning(I18n::Message::StorageMemoryFull1, I18n::Message::StorageMemoryFull2, true);
  }
}

Window * AppsContainer::window() {
  return &m_window;
}

int AppsContainer::numberOfContainerTimers() {
  return 3;
}

Timer * AppsContainer::containerTimerAtIndex(int i) {
  Timer * timers[3] = {&m_batteryTimer, &m_suspendTimer, &m_backlightDimmingTimer};
  return timers[i];
}

void AppsContainer::resetShiftAlphaStatus() {
  Ion::Events::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  updateAlphaLock();
}
