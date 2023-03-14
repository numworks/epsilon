#include "apps_container.h"

#include <escher/clipboard.h>
#include <ion.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/init.h>

#include "apps_container_storage.h"
#include "global_preferences.h"
#include "shared/record_restrictive_extensions_helper.h"

extern "C" {
#include <assert.h>
}

using namespace Shared;
using namespace Escher;
using namespace Poincare;

AppsContainer* AppsContainer::sharedAppsContainer() {
  return AppsContainerStorage::sharedAppsContainerStorage;
}

AppsContainer::AppsContainer()
    : Container(),
      m_firstUSBEnumeration(true),
      m_examPopUpController(),
      m_promptController(k_promptMessages, k_promptColors,
                         k_promptNumberOfMessages)
#if EPSILON_GETOPT
      ,
      m_initialAppSnapshot(nullptr)
#endif
{
  m_emptyBatteryWindow.setFrame(KDRectScreen, false);
  Ion::Storage::FileSystem::sharedFileSystem->setDelegate(this);
  Shared::RecordRestrictiveExtensions::
      registerRestrictiveExtensionsToSharedStorage();
}

Ion::ExternalApps::App AppsContainer::externalAppAtIndex(int index) {
  for (Ion::ExternalApps::App a : Ion::ExternalApps::Apps()) {
    if (index == 0) {
      return a;
    }
    index--;
  }
  assert(false);
  return Ion::ExternalApps::App(nullptr);
}

App::Snapshot* AppsContainer::hardwareTestAppSnapshot() {
  return &m_hardwareTestSnapshot;
}

App::Snapshot* AppsContainer::onBoardingAppSnapshot() {
  return &m_onBoardingSnapshot;
}

App::Snapshot* AppsContainer::usbConnectedAppSnapshot() {
  return &m_usbConnectedSnapshot;
}

void AppsContainer::setExamMode(Poincare::ExamMode targetExamMode) {
  ExamMode::Ruleset previousRules =
      Preferences::sharedPreferences->examMode().ruleset();
  Preferences::sharedPreferences->setExamMode(targetExamMode);

  if (targetExamMode.ruleset() != ExamMode::Ruleset::Off) {
    // Empty storage (delete functions, variables, python scripts)
    Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
    // Empty clipboard
    Clipboard::SharedClipboard()->reset();
    for (int i = 0; i < numberOfBuiltinApps(); i++) {
      appSnapshotAtIndex(i)->reset();
    }
  } else if (previousRules == ExamMode::Ruleset::PressToTest) {
    // Reset when leaving PressToTest mode.
    Ion::Reset::core();
  }

  refreshPreferences();
  App* app = activeApp();
  if (app) {
    app->modalViewController()->dismissModal();
    if (app->snapshot() != onBoardingAppSnapshot()) {
      switchToBuiltinApp(homeAppSnapshot());
    }
  }
}

Shared::GlobalContext* AppsContainer::globalContext() {
  return &m_globalContext;
}

MathToolbox* AppsContainer::mathToolbox() { return &m_mathToolbox; }

MathVariableBoxController* AppsContainer::variableBoxController() {
  return &m_variableBoxController;
}

void AppsContainer::didSuspend(bool checkIfOnOffKeyReleased) {
  resetShiftAlphaStatus();
  GlobalPreferences* globalPreferences =
      GlobalPreferences::sharedGlobalPreferences;
  // Display the prompt if it has a message to display
  if (promptController() != nullptr &&
      s_activeApp->snapshot() != onBoardingAppSnapshot() &&
      s_activeApp->snapshot() != hardwareTestAppSnapshot() &&
      globalPreferences->showPopUp()) {
    s_activeApp->displayModalViewController(promptController(), 0.f, 0.f);
  }
  /* Reset first enumeration flag in case the device was unplugged during its
   * off time. */
  m_firstUSBEnumeration = true;
  /* Ion::Power::suspend() completely shuts down the LCD controller. Therefore
   * the frame memory is lost. That's why we need to force a window redraw
   * upon wakeup, otherwise the screen is filled with noise. */
  Ion::Backlight::setBrightness(globalPreferences->brightnessLevel());
  m_backlightDimmingTimer.reset();
  m_window.updateBatteryAnimation();
  window()->redraw(true);
}

bool AppsContainer::dispatchEvent(Ion::Events::Event event) {
  bool alphaLockWantsRedraw = updateAlphaLock();
  if (event == Ion::Events::USBEnumeration || event == Ion::Events::USBPlug ||
      event == Ion::Events::BatteryCharging) {
    Ion::LED::updateColorWithPlugAndCharge();
  }

  bool didProcessEvent = Container::dispatchEvent(event);

  if (!didProcessEvent) {
    didProcessEvent = processEvent(event);
  }
  if (event.isKeyPress()) {
    m_backlightDimmingTimer.reset();
    m_suspendTimer.reset();
    Ion::Backlight::setBrightness(
        GlobalPreferences::sharedGlobalPreferences->brightnessLevel());
  }
  if (!didProcessEvent && alphaLockWantsRedraw) {
    window()->redraw();
    return true;
  }
  return didProcessEvent || alphaLockWantsRedraw;
}

bool AppsContainer::processEvent(Ion::Events::Event event) {
  if (event == Ion::Events::USBEnumeration) {
    if (!Ion::USB::isPlugged()) {
      /* Sometimes, the device gets an ENUMDNE interrupts when being unplugged
       * from a non-USB communicating host (e.g. a USB charger). The interrupt
       * must me cleared: if not the next enumeration attempts will not be
       * detected. */
      Ion::USB::clearEnumerationInterrupt();
      return false;
    }
    if (!Preferences::sharedPreferences->examMode().isActive()) {
      App::Snapshot* activeSnapshot =
          (s_activeApp == nullptr ? appSnapshotAtIndex(0)
                                  : s_activeApp->snapshot());
      /* Just after a software update, the battery timer does not have time to
       * fire before the calculator enters DFU mode. As the DFU mode blocks the
       * event loop, we update the battery state "manually" here.
       * We do it before switching to USB application to redraw the battery
       * pictogram. */
      updateBatteryState();
      switchToBuiltinApp(usbConnectedAppSnapshot());
      Ion::USB::DFU();
      // Update LED when exiting DFU mode
      Ion::LED::updateColorWithPlugAndCharge();
      switchToBuiltinApp(activeSnapshot);
    } else if (m_firstUSBEnumeration) {
      displayExamModePopUp(ExamMode(ExamMode::Ruleset::Off));
      // Warning: if the window is dirtied, you need to call window()->redraw()
      window()->redraw();
    }
    m_firstUSBEnumeration = false;
    return true;
  }
  if (event == Ion::Events::USBPlug) {
    if (Ion::USB::isPlugged()) {
      Ion::USB::enable();
      Ion::Backlight::setBrightness(
          GlobalPreferences::sharedGlobalPreferences->brightnessLevel());
    } else {
      m_firstUSBEnumeration = true;
      Ion::USB::disable();
      Ion::USB::clearEnumerationInterrupt();
    }
    return true;
  }
  if (event == Ion::Events::Home ||
      (event == Ion::Events::Back && !Ion::Events::isRepeating())) {
    switchToBuiltinApp(appSnapshotAtIndex(0));
    return true;
  }
  if (event == Ion::Events::OnOff) {
    didSuspend(true);
    return true;
  }
  return false;
}

void AppsContainer::switchToBuiltinApp(App::Snapshot* snapshot) {
  if (s_activeApp && snapshot != s_activeApp->snapshot()) {
    resetShiftAlphaStatus();
  }
  if (snapshot == hardwareTestAppSnapshot() ||
      snapshot == onBoardingAppSnapshot()) {
    m_window.hideTitleBarView(true);
  } else {
    m_window.hideTitleBarView(false);
  }
  if (snapshot) {
    m_window.setTitle(snapshot->descriptor()->upperName());
  }
  return Container::switchToBuiltinApp(snapshot);
}

typedef void (*ExternalAppMain)();

void AppsContainer::switchToExternalApp(Ion::ExternalApps::App app) {
  // TODO: should we set stricter MPU bounds?
  Container::switchToBuiltinApp(nullptr);
  reloadTitleBarView();
  Ion::Events::setSpinner(false);
  Ion::Display::setScreenshotCallback(nullptr);
  ExternalAppMain appStart =
      reinterpret_cast<ExternalAppMain>(app.entryPoint());
  if (appStart) {
    appStart();
  }
  switchToBuiltinApp(appSnapshotAtIndex(0));
  assert(s_activeApp);
  if (!appStart) {
    s_activeApp->displayWarning(I18n::Message::ExternalAppIncompatible1,
                                I18n::Message::ExternalAppIncompatible2, true);
  }
}

void AppsContainer::handleRunException(bool resetSnapshot) {
  if (s_activeApp != nullptr) {
    /* The app models can reference layouts or expressions that have been
     * destroyed from the pool. To avoid using them before packing the app
     * (in App::willBecomeInactive for instance), we tidy them early on. */
    s_activeApp->snapshot()->tidy();
    if (resetSnapshot) {
      /* When an app encountered an exception due to a full pool, the next time
       * the user enters the app, the same exception could happen again which
       * would prevent from reopening the app. To avoid being stuck outside the
       * app causing the issue, we reset its snapshot when leaving it due to
       * exception. For instance, the calculation app can encounter an
       * exception when displaying too many huge layouts, if we don't clean the
       * history here, we will be stuck outside the calculation app. */
      s_activeApp->snapshot()->reset();
    }
    if (s_activeApp->snapshot() == homeAppSnapshot()) {
      // Reset home selection if already selected
      dispatchEvent(Ion::Events::Back);
    }
  }
  switchToBuiltinApp(appSnapshotAtIndex(0));
}

void AppsContainer::run() {
  window()->setFrame(KDRectScreen, false);
  Preferences* poincarePreferences = Preferences::sharedPreferences;
  Poincare::ExamMode examMode = poincarePreferences->examMode();
  if (examMode.isActive()) {
    setExamMode(examMode);
  } else {
    refreshPreferences();
  }
  Ion::Power::selectStandbyMode(false);
  Ion::Events::setSpinner(true);
  Ion::Display::setScreenshotCallback(ShowCursor);

  /* Setup the home checkpoint so that the exception chekpoint will be
   * reactivated on a home interrupt. This way, the main exception checkpoint
   * will keep the home checkpoint as parent. */
  bool homeInterruptOcurred;
  CircuitBreakerCheckpoint homeCheckpoint(
      Ion::CircuitBreaker::CheckpointType::Home);
  if (CircuitBreakerRun(homeCheckpoint)) {
    homeInterruptOcurred = false;
  } else {
    homeInterruptOcurred = true;
  }

  ExceptionCheckpoint exceptionCheckpoint;
  if (ExceptionRun(exceptionCheckpoint)) {
    if (homeInterruptOcurred) {
      /* Reset backlight and suspend timers here, because a keyboard event has
       * loaded the checkpoint and did not call AppsContainer::dispatchEvent */
      m_backlightDimmingTimer.reset();
      m_suspendTimer.reset();
      Ion::Backlight::setBrightness(
          GlobalPreferences::sharedGlobalPreferences->brightnessLevel());
      Ion::Events::setSpinner(true);
      Ion::Display::setScreenshotCallback(ShowCursor);
      handleRunException(false);
    } else {
      /* Normal execution. The exception checkpoint must be created before
       * switching to the first app, because the first app might create nodes on
       * the pool. */
      switchToBuiltinApp(initialAppSnapshot());
    }
  } else {
    /* We lock the Poincare pool until the application is destroyed (the pool
     * is then asserted empty). This prevents from allocating new handles
     * with the same identifiers as potential dangling handles (that have
     * lost their nodes in the exception). */
    TreePool::Lock();
    handleRunException(true);
    TreePool::Unlock();
    s_activeApp->displayWarning(I18n::Message::PoolMemoryFull1,
                                I18n::Message::PoolMemoryFull2, true);
  }

  Container::run();
  switchToBuiltinApp(nullptr);
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

void AppsContainer::refreshPreferences() { m_window.refreshPreferences(); }

void AppsContainer::reloadTitleBarView() { m_window.reloadTitleBarView(); }

void AppsContainer::displayExamModePopUp(ExamMode mode) {
  m_examPopUpController.setTargetExamMode(mode);
  m_examPopUpController.presentModally();
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
  while (Ion::Battery::level() == Ion::Battery::Charge::EMPTY &&
         !Ion::USB::isPlugged()) {
    Ion::Backlight::setBrightness(0);
    m_emptyBatteryWindow.redraw(true);
    Ion::Timing::msleep(3000);
    Ion::Power::suspend();
  }
}

void AppsContainer::setShiftAlphaStatus(
    Ion::Events::ShiftAlphaStatus newStatus) {
  Ion::Events::setShiftAlphaStatus(newStatus);
  updateAlphaLock();
}

bool AppsContainer::updateAlphaLock() { return m_window.updateAlphaLock(); }

OnBoarding::PromptController* AppsContainer::promptController() {
  if (k_promptNumberOfMessages == 0) {
    return nullptr;
  }
  return &m_promptController;
}

void AppsContainer::redrawWindow() { m_window.redraw(); }

bool AppsContainer::storageCanChangeForRecordName(
    const Ion::Storage::Record::Name recordName) const {
  if (s_activeApp) {
    return s_activeApp->storageCanChangeForRecordName(recordName);
  }
  return true;
}

void AppsContainer::storageDidChangeForRecord(
    const Ion::Storage::Record record) {
  m_globalContext.storageDidChangeForRecord(record);
  if (s_activeApp) {
    s_activeApp->storageDidChangeForRecord(record);
  }
}

void AppsContainer::storageIsFull() {
  if (s_activeApp) {
    s_activeApp->displayWarning(I18n::Message::StorageMemoryFull1,
                                I18n::Message::StorageMemoryFull2, true);
  }
}

Window* AppsContainer::window() { return &m_window; }

int AppsContainer::numberOfContainerTimers() { return 4; }

Timer* AppsContainer::containerTimerAtIndex(int i) {
  Timer* timers[4] = {&m_batteryTimer, &m_suspendTimer,
                      &m_backlightDimmingTimer, &m_blinkTimer};
  return timers[i];
}

void AppsContainer::resetShiftAlphaStatus() {
  Ion::Events::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  updateAlphaLock();
}

void AppsContainer::ShowCursor() {
  AppsContainer* container = sharedAppsContainer();
  container->m_blinkTimer.forceCursorVisible();
  container->window()->redraw();
}
