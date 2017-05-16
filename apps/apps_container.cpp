#include "apps_container.h"
#include "global_preferences.h"
#include <ion.h>

extern "C" {
#include <assert.h>
}

using namespace Shared;

AppsContainer::AppsContainer() :
  Container(),
  m_window(AppsWindow()),
  m_emptyBatteryWindow(EmptyBatteryWindow()),
  m_globalContext(),
  m_variableBoxController(&m_globalContext),
  m_examPopUpController(ExamPopUpController()),
  m_updateController(),
  m_ledTimer(LedTimer()),
  m_batteryTimer(BatteryTimer(this)),
  m_USBTimer(USBTimer(this)),
  m_suspendTimer(SuspendTimer(this)),
  m_backlightDimmingTimer(BacklightDimmingTimer())
{
  m_descriptors[0] = Home::App::buildDescriptor();
  m_descriptors[1] = Calculation::App::buildDescriptor();
  m_descriptors[2] = Graph::App::buildDescriptor();
  m_descriptors[3] = Sequence::App::buildDescriptor();
  m_descriptors[4] = Settings::App::buildDescriptor();
  m_descriptors[5] = Statistics::App::buildDescriptor();
  m_descriptors[6] = Probability::App::buildDescriptor();
  m_descriptors[7] = Regression::App::buildDescriptor();
  m_descriptors[8] = OnBoarding::App::buildDescriptor();
  m_descriptors[9] = HardwareTest::App::buildDescriptor();
  refreshPreferences();
  m_emptyBatteryWindow.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  Poincare::Expression::setCircuitBreaker(AppsContainer::poincareCircuitBreaker);
}

AppsContainer::~AppsContainer() {
  for (int i = 0; i < k_totalNumberOfApps; i++) {
    delete m_descriptors[i];
  }
}

bool AppsContainer::poincareCircuitBreaker(const Poincare::Expression * e) {
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  return Ion::Keyboard::keyDown(Ion::Keyboard::Key::A6, state);
}

int AppsContainer::numberOfApps() {
  return k_numberOfCommonApps;
}

App::Descriptor * AppsContainer::appDescriptorAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfCommonApps);
  return m_descriptors[index];
}

App::Descriptor * AppsContainer::hardwareTestAppDescriptor() {
  return m_descriptors[k_totalNumberOfApps-1];
}

App::Descriptor * AppsContainer::onBoardingAppDescriptor() {
  return m_descriptors[k_totalNumberOfApps - 2];
}

void AppsContainer::reset() {
  Clipboard::sharedClipboard()->reset();
  //TODO: persitence->reset()
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
  if (activeApp()->descriptor() != onBoardingAppDescriptor() && GlobalPreferences::sharedGlobalPreferences()->showUpdatePopUp()) {
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
  if (event == Ion::Events::Home && activeApp()->descriptor() != onBoardingAppDescriptor() && activeApp()->descriptor() != hardwareTestAppDescriptor() && activeApp()->descriptor() != appDescriptorAtIndex(0)) {
    switchTo(appDescriptorAtIndex(0));
    return true;
  }
  if (event == Ion::Events::OnOff && activeApp()->descriptor() != hardwareTestAppDescriptor()) {
    if (activeApp()->descriptor() == onBoardingAppDescriptor()) {
      ((OnBoarding::App *)activeApp())->reinitOnBoarding();
    }
    suspend(true);
    return true;
  }
  bool didProcessEvent = Container::dispatchEvent(event);
  if (!didProcessEvent && event == Ion::Events::Back) {
    switchTo(appDescriptorAtIndex(0));
    return true;
  }
  if (!didProcessEvent && alphaLockWantsRedraw) {
    windowRedraw();
    return true;
  }
  return didProcessEvent;
}

void AppsContainer::switchTo(App::Descriptor * descriptor) {
  if (descriptor == hardwareTestAppDescriptor() || descriptor == onBoardingAppDescriptor()) {
    m_window.hideTitleBarView(true);
  } else {
    m_window.hideTitleBarView(false);
  }
  if (descriptor) {
    m_window.setTitle(descriptor->upperName());
  }
  Container::switchTo(descriptor);
  if (activeApp()->descriptor() == onBoardingAppDescriptor()) {
    ((OnBoarding::App *)activeApp())->reinitOnBoarding();
  }
}

void AppsContainer::updateBatteryState() {
  m_window.updateBatteryLevel();
  m_window.updateIsChargingState();
  m_window.updatePluggedState();
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
  bool onBoardingTimer = activeApp()->descriptor() == onBoardingAppDescriptor() && ((OnBoarding::App *)activeApp())->hasTimer();
  return 4+(GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) + onBoardingTimer;
}

Timer * AppsContainer::timerAtIndex(int i) {
  bool onBoardingTimer = activeApp()->descriptor() == onBoardingAppDescriptor() && ((OnBoarding::App *)activeApp())->hasTimer();
  if (onBoardingTimer && i == 4) {
    return ((OnBoarding::App *)activeApp())->timer();
  }
  Timer * timers[6] = {&m_batteryTimer, &m_USBTimer, &m_suspendTimer, &m_backlightDimmingTimer, &m_ledTimer, &m_ledTimer};
  return timers[i];
}
