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
  m_examPopUpController(this),
  m_updateController(),
  m_ledTimer(LedTimer()),
  m_batteryTimer(BatteryTimer(this)),
  m_suspendTimer(SuspendTimer(this)),
  m_backlightDimmingTimer(),
  m_homeSnapshot(),
  m_onBoardingSnapshot(),
  m_hardwareTestSnapshot(),
  m_usbConnectedSnapshot()
{
  m_emptyBatteryWindow.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  Poincare::Expression::setCircuitBreaker(AppsContainer::poincareCircuitBreaker);
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
#if EPSILON_SOFTWARE_UPDATE_PROMPT
  if (activeApp()->snapshot()!= onBoardingAppSnapshot() && GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp()) {
    activeApp()->displayModalViewController(&m_updateController, 0.f, 0.f);
  }
#endif
  Ion::Power::suspend(checkIfPowerKeyReleased);
  /* Ion::Power::suspend() completely shuts down the LCD controller. Therefore
   * the frame memory is lost. That's why we need to force a window redraw
   * upon wakeup, otherwise the screen is filled with noise. */
  Ion::Backlight::setBrightness(GlobalPreferences::sharedGlobalPreferences()->brightnessLevel());
  m_backlightDimmingTimer.reset();
  window()->redraw(true);
}

bool AppsContainer::dispatchEvent(Ion::Events::Event event) {
  bool alphaLockWantsRedraw = updateAlphaLock();

  bool didProcessEvent = false;

  if (event == Ion::Events::USBPlug) {
    if (Ion::USB::isPlugged()) {
      if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
        displayExamModePopUp(false);
      } else {
        Ion::USB::enable();
      }
      Ion::Backlight::setBrightness(Ion::Backlight::MaxBrightness);
    } else {
      Ion::USB::disable();
    }
    didProcessEvent = true;
  } else if (event == Ion::Events::USBEnumeration) {
    if (Ion::USB::isPlugged()) {
      App::Snapshot * activeSnapshot = (activeApp() == nullptr ? appSnapshotAtIndex(0) : activeApp()->snapshot());
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
#if EPSILON_ONBOARDING_APP
  switchTo(onBoardingAppSnapshot());
#else
  if (numberOfApps() == 2) {
    switchTo(appSnapshotAtIndex(1));
  } else {
    switchTo(appSnapshotAtIndex(0));
  }
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

void AppsContainer::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus newStatus) {
  Ion::Events::setShiftAlphaStatus(newStatus);
  updateAlphaLock();
}

bool AppsContainer::updateAlphaLock() {
  return m_window.updateAlphaLock();
}

OnBoarding::UpdateController * AppsContainer::updatePopUpController() {
  return &m_updateController;
}

void AppsContainer::redrawWindow() {
  m_window.redraw();
}

void AppsContainer::examDeactivatingPopUpIsDismissed() {
  if (Ion::USB::isPlugged()) {
    Ion::USB::enable();
  }
}

Window * AppsContainer::window() {
  return &m_window;
}

int AppsContainer::numberOfContainerTimers() {
  return 3+(GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate);
}

Timer * AppsContainer::containerTimerAtIndex(int i) {
  Timer * timers[4] = {&m_batteryTimer, &m_suspendTimer, &m_backlightDimmingTimer, &m_ledTimer};
  return timers[i];
}

void AppsContainer::resetShiftAlphaStatus() {
  Ion::Events::setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus::Default);
  updateAlphaLock();
}
