#include "apps_container.h"
#include "apps_container_storage.h"
#include "global_preferences.h"
#include "exam_mode_configuration.h"
#include <ion.h>
#include <poincare/init.h>
#include <poincare/exception_checkpoint.h>
#include <ion/backlight.h>
#include <poincare/preferences.h>

#include <algorithm>

extern "C" {
#include <assert.h>
}

using namespace Poincare;
using namespace Shared;

AppsContainer * AppsContainer::sharedAppsContainer() {
  static AppsContainerStorage appsContainerStorage;
  return &appsContainerStorage;
}

AppsContainer::AppsContainer() :
  Container(),
  m_window(),
  m_emptyBatteryWindow(),
  m_globalContext(),
  m_variableBoxController(),
  m_examPopUpController(this),
  m_promptController(k_promptMessages, k_promptFGColors, k_promptBGColors, k_promptNumberOfMessages),
  m_batteryTimer(),
  m_suspendTimer(),
  m_backlightDimmingTimer(),
  m_clockTimer(ClockTimer(this)),
  m_homeSnapshot(),
  m_onBoardingSnapshot(),
  m_hardwareTestSnapshot(),
  m_usbConnectedSnapshot(),
  m_startAppSnapshot()
{
  m_emptyBatteryWindow.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), false);
// #if __EMSCRIPTEN__
  /* AppsContainer::poincareCircuitBreaker uses Ion::Keyboard::scan(), which
   * calls emscripten_sleep. If we set the poincare circuit breaker, we would
   * need to whitelist all the methods that might be in the call stack when
   * poincareCircuitBreaker is run. This means either whitelisting all Epsilon
   * (which makes bigger files to download and slower execution), or
   * whitelisting all the symbols (that's a big amount of symbols to find and
   * quite paint to maintain).
   * We just remove the circuit breaker for now.
   * TODO: Put the Poincare circuit breaker back on epsilon's web emulator */

  /*
   * This can be run in Omega, since it uses WebASM.
   */
// #else
  Poincare::Expression::SetCircuitBreaker(AppsContainer::poincareCircuitBreaker);
// #endif
  Ion::Storage::sharedStorage()->setDelegate(this);

  addTimer(&m_batteryTimer);
  addTimer(&m_suspendTimer);
  addTimer(&m_backlightDimmingTimer);
  addTimer(&m_clockTimer);
}

bool AppsContainer::poincareCircuitBreaker() {
  constexpr uint64_t minimalPressDuration = 20;
  static uint64_t beginningOfInterruption = 0;
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  bool interrupt = state.keyDown(Ion::Keyboard::Key::Back) || state.keyDown(Ion::Keyboard::Key::Home) || state.keyDown(Ion::Keyboard::Key::OnOff);
  if (!interrupt) {
    beginningOfInterruption = 0;
    return false;
  }
  if (beginningOfInterruption == 0) {
    beginningOfInterruption = Ion::Timing::millis();
    return false;
  }
  if (Ion::Timing::millis() - beginningOfInterruption > minimalPressDuration) {
    beginningOfInterruption = 0;
    return true;
  }
  return false;
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

MathVariableBoxController * AppsContainer::variableBoxController() {
  return &m_variableBoxController;
}

void AppsContainer::suspend(bool checkIfOnOffKeyReleased) {
  resetShiftAlphaStatus();
  GlobalPreferences * globalPreferences = GlobalPreferences::sharedGlobalPreferences();
  // Display the prompt if it has a message to display
  if (promptController() != nullptr && s_activeApp->snapshot()!= onBoardingAppSnapshot() && s_activeApp->snapshot() != hardwareTestAppSnapshot() && globalPreferences->showPopUp()) {
    s_activeApp->displayModalViewController(promptController(), 0.f, 0.f);
  }
  Ion::Power::suspend(checkIfOnOffKeyReleased);
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

  if (event == Ion::Events::USBEnumeration || event == Ion::Events::USBPlug || event == Ion::Events::BatteryCharging) {
    Ion::LED::updateColorWithPlugAndCharge();
  }
  if (event == Ion::Events::USBEnumeration) {
    if (Ion::USB::isPlugged()) {
      App::Snapshot * activeSnapshot = (s_activeApp == nullptr ? appSnapshotAtIndex(0) : s_activeApp->snapshot());
      /* Just after a software update, the battery timer does not have time to
       * fire before the calculator enters DFU mode. As the DFU mode blocks the
       * event loop, we update the battery state "manually" here.
       * We do it before switching to USB application to redraw the battery
       * pictogram. */
      updateBatteryState();
      if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
        // If we are in exam mode, we don't switch to usb connected app
        didProcessEvent = true;
      } else if (switchTo(usbConnectedAppSnapshot())) {
        Ion::USB::DFU(true);
        // Update LED when exiting DFU mode
        Ion::LED::updateColorWithPlugAndCharge();
        bool switched = switchTo(activeSnapshot);
        assert(switched);
        (void) switched; // Silence compilation warning about unused variable.
        didProcessEvent = true;
      } else {
        /* We could not switch apps, which means that the current app needs
         * another event loop to prepare for being switched off.
         * Discard the current enumeration interruption.
         * The USB host tries a few times in a row to enumerate the device, so
         * hopefully the device will get another enumeration event soon and this
         * time the device will be ready to go in DFU mode. Otherwise, the user
         * needs to re-plug the device to go into DFU mode. */
        Ion::USB::clearEnumerationInterrupt();
      }
    } else {
      /* Sometimes, the device gets an ENUMDNE interrupts when being unplugged
       * from a non-USB communicating host (e.g. a USB charger). The interrupt
       * must me cleared: if not the next enumeration attempts will not be
       * detected. */
      Ion::USB::clearEnumerationInterrupt();
    }
  } else {
    if (KDIonContext::sharedContext()->zoomEnabled) {
      bool changedZoom = true;

      if (event == Ion::Events::ShiftOne) {
        KDIonContext::sharedContext()->zoomPosition = 0;
      } else if (event == Ion::Events::ShiftTwo) {
        KDIonContext::sharedContext()->zoomPosition = 1;
      } else if (event == Ion::Events::ShiftThree) {
        KDIonContext::sharedContext()->zoomPosition = 2;
      } else if (event == Ion::Events::ShiftFour) {
        KDIonContext::sharedContext()->zoomPosition = 3;
      } else if (event == Ion::Events::ShiftFive) {
        KDIonContext::sharedContext()->zoomPosition = 4;
      } else if (event == Ion::Events::ShiftSix) {
        KDIonContext::sharedContext()->zoomPosition = 5;
      } else if (event == Ion::Events::ShiftSeven) {
        KDIonContext::sharedContext()->zoomPosition = 6;
      } else if (event == Ion::Events::ShiftEight) {
        KDIonContext::sharedContext()->zoomPosition = 7;
      } else if (event == Ion::Events::ShiftNine) {
        KDIonContext::sharedContext()->zoomPosition = 8;
      } else {
        changedZoom = false;
      }
      if (changedZoom) {
        KDIonContext::sharedContext()->updatePostProcessingEffects();
        redrawWindow(true);
        return true;
      }
    }
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

// List of keys that are used to switch between apps, in order of app to go (eg. 0 : First App, 1 : Second App, 2 : Third App, ...)
static constexpr Ion::Events::Event switch_events[] = {
    Ion::Events::ShiftSeven, Ion::Events::ShiftEight, Ion::Events::ShiftNine,
    Ion::Events::ShiftFour, Ion::Events::ShiftFive, Ion::Events::ShiftSix,
    Ion::Events::ShiftOne, Ion::Events::ShiftTwo, Ion::Events::ShiftThree,
    Ion::Events::ShiftZero, Ion::Events::ShiftDot, Ion::Events::ShiftEE
};

bool AppsContainer::processEvent(Ion::Events::Event event) {
  // Warning: if the window is dirtied, you need to call window()->redraw()
  if (event == Ion::Events::USBPlug) {
    if (Ion::USB::isPlugged()) {
      // If the exam mode is enabled, we ask to disable it, else, we enable USB
      if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
        displayExamModePopUp(GlobalPreferences::ExamMode::Off);
        window()->redraw();
      } else {
        Ion::USB::enable();
      }
      // Update brightness when USB is plugged
      Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
    } else {
      // If the USB isn't plugged in USBPlug event, we disable USB
      Ion::USB::disable();
    }
    return true;
  }
  // If key home or key back is pressed, we switch to the home app.
  if (event == Ion::Events::Home || event == Ion::Events::Back) {
    switchTo(appSnapshotAtIndex(0));
    return true;
  }
  // If shift + Home are pressed, we switch to the first app.
  if (event == Ion::Events::ShiftHome) {
    switchTo(appSnapshotAtIndex(1));
    return true;
  }

  // Iterate through the switch events to find the one that matches the event, if one match, switch to the app at the index of the switch event.
  for(int i = 0; i < std::min((int) (sizeof(switch_events) / sizeof(Ion::Events::Event)), APPS_CONTAINER_SNAPSHOT_COUNT); i++) {
    if (event == switch_events[i]) {
      m_window.redraw(true);
      switchTo(appSnapshotAtIndex(i+1));
      return true;
    }
  }

  // Add EE shortcut to go to the settings (app number 12)
  if (event == Ion::Events::EE) {
    switchTo(appSnapshotAtIndex(12));
    return true;
  }

  // Add Shift + Ans shortcut to go to the previous app
  if (event == Ion::Events::ShiftAns) {
    switchTo(appSnapshotAtIndex(m_lastAppIndex));
    return true;
  }

  // If the event is the OnOff key, we suspend the calculator.
  if (event == Ion::Events::OnOff) {
    suspend(true);
    return true;
  }
  // If the event is a brightness event, we update the brightness according to the event.
  if (event == Ion::Events::BrightnessPlus || event == Ion::Events::BrightnessMinus) {
      int delta = Ion::Backlight::MaxBrightness/GlobalPreferences::NumberOfBrightnessStates;
      int NumberOfStepsPerShortcut = GlobalPreferences::sharedGlobalPreferences()->brightnessShortcut();
      int direction = (event == Ion::Events::BrightnessPlus) ? NumberOfStepsPerShortcut*delta : -delta*NumberOfStepsPerShortcut;
      GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel()+direction);
  }
  // Else, the event was not processed.
  return false;
}

bool AppsContainer::switchTo(App::Snapshot * snapshot) {
  // Get app index of the snapshot
  int m_appIndexToSwitch = appIndexFromSnapshot(snapshot);
  // If the app is home, skip app index saving
  if (m_appIndexToSwitch != 0) {
    // Save last app index
    m_lastAppIndex = m_currentAppIndex;
    // Save current app index
    m_currentAppIndex = m_appIndexToSwitch;
  }
  if (s_activeApp && snapshot != s_activeApp->snapshot()) {
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
  return Container::switchTo(snapshot);
}

void AppsContainer::run() {
  KDRect screenRect = KDRect(0, 0, Ion::Display::Width, Ion::Display::Height);
  window()->setFrame(screenRect, false);
  /* We push a white screen here, because fetching the exam mode takes some time
   * and it is visible when reflashing a N0100 (there is some noise on the
   * screen before the logo appears). */
  Ion::Display::pushRectUniform(screenRect, KDColorWhite);
  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    activateExamMode(GlobalPreferences::sharedGlobalPreferences()->examMode());
  }
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
    bool switched;
    if (m_startAppSnapshot != nullptr) {
      switched = switchTo(m_startAppSnapshot);
    } else {
      switched = switchTo(initialAppSnapshot());
    }

    assert(switched);
    (void) switched; // Silence compilation warning about unused variable.
  } else {
    // Exception
    if (s_activeApp != nullptr) {
      /* The app models can reference layouts or expressions that have been
       * destroyed from the pool. To avoid using them before packing the app
       * (in App::willBecomeInactive for instance), we tidy them early on. */
      s_activeApp->snapshot()->tidy();
      /* When an app encountered an exception due to a full pool, the next time
       * the user enters the app, the same exception could happen again which
       * would prevent from reopening the app. To avoid being stuck outside the
       * app causing the issue, we reset its snapshot when leaving it due to
       * exception. For instance, the calculation app can encounter an
       * exception when displaying too many huge layouts, if we don't clean the
       * history here, we will be stuck outside the calculation app. */
      s_activeApp->snapshot()->reset();
    }
    bool switched = switchTo(appSnapshotAtIndex(0));
    assert(switched);
    (void) switched; // Silence compilation warning about unused variable.
    Poincare::Tidy();
    s_activeApp->displayWarning(I18n::Message::PoolMemoryFull1, I18n::Message::PoolMemoryFull2, true);
  }
  Container::run();
  switchTo(nullptr);
}

bool AppsContainer::updateClock() {
  return m_window.updateClock();
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
  m_suspendTimer.reset(GlobalPreferences::sharedGlobalPreferences()->idleBeforeSuspendSeconds()*1000/Timer::TickDuration);
  m_backlightDimmingTimer.reset(GlobalPreferences::sharedGlobalPreferences()->idleBeforeDimmingSeconds()*1000/Timer::TickDuration);
  m_window.refreshPreferences();
}

void AppsContainer::reloadTitleBarView() {
  m_window.reloadTitleBarView();
}

void AppsContainer::displayExamModePopUp(GlobalPreferences::ExamMode mode) {
  m_examPopUpController.setTargetExamMode(mode);
  s_activeApp->displayModalViewController(&m_examPopUpController, 0.f, 0.f, Metric::ExamPopUpTopMargin, Metric::PopUpRightMargin, Metric::ExamPopUpBottomMargin, Metric::PopUpLeftMargin);
}

void AppsContainer::shutdownDueToLowBattery() {
  if (Ion::Battery::level() != Ion::Battery::Charge::EMPTY) {
  /* We early escape here. When the battery switches from LOW to EMPTY, it
   * oscillates a few times before stabilizing to EMPTY. So we might call
   * 'shutdownDueToLowBattery' but the battery level still answers LOW instead
   * of EMPTY. We want to avoid uselessly redrawing the whole window in that
   * case. */
    return;
  }
  while (Ion::Battery::level() == Ion::Battery::Charge::EMPTY && !Ion::USB::isPlugged()) {
    Ion::Backlight::setBrightness(0);
    if (!GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
      /* Unless the LED is lit up for the exam mode, switch off the LED. IF the
       * low battery event happened during the Power-On Self-Test, a LED might
       * have stayed lit up. */
      Ion::LED::setColor(KDColorBlack);
    }
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

OnBoarding::PromptController * AppsContainer::promptController() {
  if (k_promptNumberOfMessages == 0) {
    return nullptr;
  }
  return &m_promptController;
}

void AppsContainer::redrawWindow(bool force) {
  m_window.redraw(force);
}

void AppsContainer::activateExamMode(GlobalPreferences::ExamMode examMode) {
  assert(examMode != GlobalPreferences::ExamMode::Off && examMode != GlobalPreferences::ExamMode::Unknown);
  reset();
  Ion::LED::setColor(KDColorRed);
  /* The Dutch exam mode LED is supposed to be orange but we can only make
   * blink "pure" colors: with RGB leds on or off (as the PWM is used for
   * blinking). The closest "pure" color is Yellow. Moreover, Orange LED is
   * already used when the battery is charging. Using yellow, we can assert
   * that the yellow LED only means that Dutch exam mode is on and avoid
   * confusing states when the battery is charging and states when the Dutch
   * exam mode is on. */
  // Ion::LED::setColor(examMode == GlobalPreferences::ExamMode::Dutch ? KDColorYellow : KDColorRed);
  Ion::LED::setBlinking(1000, 0.1f);
}

void AppsContainer::examDeactivatingPopUpIsDismissed() {
  if (Ion::USB::isPlugged()) {
    Ion::USB::enable();
  }
}

void AppsContainer::storageDidChangeForRecord(const Ion::Storage::Record record) {
  if (s_activeApp) {
    s_activeApp->snapshot()->storageDidChangeForRecord(record);
  }
}

void AppsContainer::storageIsFull() {
  if (s_activeApp) {
    s_activeApp->displayWarning(I18n::Message::StorageMemoryFull1, I18n::Message::StorageMemoryFull2, true);
  }
}

Window * AppsContainer::window() {
  return &m_window;
}

void AppsContainer::resetShiftAlphaStatus() {
  Ion::Events::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  updateAlphaLock();
}
