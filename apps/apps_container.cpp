#include "apps_container.h"
#include "global_preferences.h"
#include <ion.h>

extern "C" {
#include <assert.h>
}

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
    &m_codeSnapshot
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
  resetShiftAlphaStatus();
#if OS_WITH_SOFTWARE_UPDATE_PROMPT
  if (activeApp()->snapshot()!= onBoardingAppSnapshot() && GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp()) {
    activeApp()->displayModalViewController(&m_updateController, 0.f, 0.f);
  }
#endif
  Ion::Power::suspend(checkIfPowerKeyReleased);
  /* Ion::Power::suspend() completely shuts down the LCD controller. Therefore
   * the frame memory is lost. That's why we need to force a window redraw
   * upon wakeup, otherwise the screen is filled with noise. */
  Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);
  m_backlightDimmingTimer.reset();
  window()->redraw(true);
}

bool AppsContainer::dispatchEvent(Ion::Events::Event event) {
  bool alphaLockWantsRedraw = m_window.updateAlphaLock();

  bool didProcessEvent = Container::dispatchEvent(event);

  if (!didProcessEvent) {
    didProcessEvent = processEvent(event);
  }
  if (event.isKeyboardEvent()) {
    m_backlightDimmingTimer.reset();
    m_suspendTimer.reset();
    Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);
  }
  if (!didProcessEvent && alphaLockWantsRedraw) {
    window()->redraw();
    return true;
  }
  return didProcessEvent || alphaLockWantsRedraw;
}

bool AppsContainer::processEvent(Ion::Events::Event event) {
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
#if OS_WITH_ONBOARDING_APP
  switchTo(onBoardingAppSnapshot());
#else
  refreshPreferences();
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

OnBoarding::UpdateController * AppsContainer::updatePopUpController() {
  return &m_updateController;
}

Window * AppsContainer::window() {
  return &m_window;
}

int AppsContainer::numberOfContainerTimers() {
  return 4+(GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate);
}

Timer * AppsContainer::containerTimerAtIndex(int i) {
  Timer * timers[5] = {&m_batteryTimer, &m_USBTimer, &m_suspendTimer, &m_backlightDimmingTimer, &m_ledTimer};
  return timers[i];
}

void AppsContainer::resetShiftAlphaStatus() {
  Ion::Keyboard::setShiftAlphaStatus(Ion::Keyboard::ShiftAlphaStatus::Default);
  m_window.updateAlphaLock();
}
