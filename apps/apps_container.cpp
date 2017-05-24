#include "apps_container.h"
#include "global_preferences.h"
#include <ion.h>

extern "C" {
#include <assert.h>
}

#ifndef ON_BOARDING
#error This file expects ON_BOARDING to be defined
#endif

using namespace Shared;

AppsContainer::AppsContainer() :
  Container(),
  m_window(),
  m_emptyBatteryWindow(),
  m_globalContext(),
  m_variableBoxController(&m_globalContext),
  m_examPopUpController(),
  m_updateController(),
  m_ledTimer(LedTimer()),
  m_batteryTimer(BatteryTimer(this)),
  m_USBTimer(USBTimer(this)),
  m_suspendTimer(SuspendTimer(this)),
  m_backlightDimmingTimer(),
  m_hardwareTestSnapshot(),
  m_onBoardingSnapshot(),
  m_homeSnapshot(),
  m_calculationSnapshot(),
  m_graphSnapshot(),
  m_sequenceSnapshot(),
  m_settingsSnapshot(),
  m_statisticsSnapshot(),
  m_probabilitySnapshot(),
  m_regressionSnapshot()
{
  refreshPreferences();
  m_emptyBatteryWindow.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  Poincare::Expression::setCircuitBreaker(AppsContainer::poincareCircuitBreaker);
}

bool AppsContainer::poincareCircuitBreaker(const Poincare::Expression * e) {
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  return state.keyDown(Ion::Keyboard::Key::A6);
}

int AppsContainer::numberOfApps() {
  return k_numberOfCommonApps;
}

App::Snapshot * AppsContainer::appSnapshotAtIndex(int index) {
  if (index < 0) {
    return nullptr;
  }
  App::Snapshot * snapshots[] = {
    &m_homeSnapshot,
    &m_calculationSnapshot,
    &m_graphSnapshot,
    &m_sequenceSnapshot,
    &m_settingsSnapshot,
    &m_statisticsSnapshot,
    &m_probabilitySnapshot,
    &m_regressionSnapshot,
  };
  assert(sizeof(snapshots)/sizeof(snapshots[0]) == k_numberOfCommonApps);
  assert(index >= 0 && index < k_numberOfCommonApps);
  return snapshots[index];
}

App::Snapshot * AppsContainer::hardwareTestAppSnapshot() {
  return &m_hardwareTestSnapshot;
}

App::Snapshot * AppsContainer::onBoardingAppSnapshot() {
  return &m_onBoardingSnapshot;
}

void AppsContainer::reset() {
  Clipboard::sharedClipboard()->reset();
  for (int i = 0; i < k_numberOfCommonApps; i++) {
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
  if (activeApp()->snapshot()!= onBoardingAppSnapshot() && GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp()) {
    activeApp()->displayModalViewController(&m_updateController, 0.f, 0.f);
  }
  Ion::Power::suspend(checkIfPowerKeyReleased);
  /* Ion::Power::suspend() completely shuts down the LCD controller. Therefore
   * the frame memory is lost. That's why we need to force a window redraw
   * upon wakeup, otherwise the screen is filled with noise. */
  Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);
  m_backlightDimmingTimer.reset();
  window()->redraw(true);
}

bool AppsContainer::dispatchEvent(Ion::Events::Event event) {
  m_backlightDimmingTimer.reset();
  m_suspendTimer.reset();
  Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);

  bool alphaLockWantsRedraw = m_window.updateAlphaLock();

  // Home and Power buttons are not sent to apps. We handle them straight away.
  if (event == Ion::Events::Home && activeApp()->snapshot() != onBoardingAppSnapshot() && activeApp()->snapshot() != hardwareTestAppSnapshot()) {
    switchTo(appSnapshotAtIndex(0));
    return true;
  }
  if (event == Ion::Events::OnOff && activeApp()->snapshot() != hardwareTestAppSnapshot()) {
    if (activeApp()->snapshot() == onBoardingAppSnapshot()) {
      ((OnBoarding::App *)activeApp())->reinitOnBoarding();
    }
    suspend(true);
    return true;
  }
  bool didProcessEvent = Container::dispatchEvent(event);
  if (!didProcessEvent && event == Ion::Events::Back) {
    switchTo(appSnapshotAtIndex(0));
    return true;
  }
  if (!didProcessEvent && alphaLockWantsRedraw) {
    window()->redraw();
    return true;
  }
  return didProcessEvent;
}

void AppsContainer::switchTo(App::Snapshot * snapshot) {
  if (snapshot == hardwareTestAppSnapshot() || snapshot == onBoardingAppSnapshot()) {
    m_window.hideTitleBarView(true);
  } else {
    m_window.hideTitleBarView(false);
  }
  if (snapshot) {
    m_window.setTitle(snapshot->descriptor()->upperName());
  }
  Container::switchTo(snapshot);
  if (activeApp() != nullptr && activeApp()->snapshot() == onBoardingAppSnapshot()) {
    ((OnBoarding::App *)activeApp())->reinitOnBoarding();
  }
}

void AppsContainer::run() {
#if ON_BOARDING
  switchTo(onBoardingAppSnapshot());
#else
  switchTo(appSnapshotAtIndex(0));
#endif
  Container::run();
  switchTo(nullptr);
}

bool AppsContainer::updateBatteryState() {
  if (m_window.updateBatteryLevel() ||
      m_window.updateIsChargingState() ||
      m_window.updatePluggedState()) {
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
    Ion::msleep(3000);
    Ion::Power::suspend();
  }
  window()->redraw(true);
}

void AppsContainer::reloadTitleBar() {
  m_window.reloadTitleBar();
}

UpdateController * AppsContainer::updatePopUpController() {
  return &m_updateController;
}

Window * AppsContainer::window() {
  return &m_window;
}

int AppsContainer::numberOfTimers() {
  bool onBoardingTimer = activeApp()->snapshot() == onBoardingAppSnapshot() && ((OnBoarding::App *)activeApp())->hasTimer();
  return 4+(GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) + onBoardingTimer;
}

Timer * AppsContainer::timerAtIndex(int i) {
  bool onBoardingTimer = activeApp()->snapshot() == onBoardingAppSnapshot() && ((OnBoarding::App *)activeApp())->hasTimer();
  if (onBoardingTimer && i == 4) {
    return ((OnBoarding::App *)activeApp())->timer();
  }
  Timer * timers[6] = {&m_batteryTimer, &m_USBTimer, &m_suspendTimer, &m_backlightDimmingTimer, &m_ledTimer, &m_ledTimer};
  return timers[i];
}
