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
  m_ledTimer(LedTimer()),
  m_batteryTimer(BatteryTimer(this)),
  m_USBTimer(USBTimer(this)),
  m_suspendTimer(SuspendTimer(this)),
  m_backlightDimmingTimer(BacklightDimmingTimer()),
  m_homeApp(new Home::App(this)),
  m_graphApp(new Graph::App(this, &m_globalContext)),
  m_probabilityApp(new Probability::App(this)),
  m_calculationApp(new Calculation::App(this, &m_globalContext)),
  m_hardwareTestApp(new HardwareTest::App(this)),
  m_regressionApp(new Regression::App(this)),
  m_sequenceApp(new Sequence::App(this, &m_globalContext)),
  m_settingsApp(new Settings::App(this)),
  m_statisticsApp(new Statistics::App(this))
{
  refreshPreferences();
  m_emptyBatteryWindow.setFrame(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height));
  Poincare::Expression::setCircuitBreaker(AppsContainer::poincareCircuitBreaker);
}

AppsContainer::~AppsContainer() {
  delete m_homeApp;
  m_homeApp = nullptr;
  delete m_graphApp;
  m_graphApp = nullptr;
  delete m_probabilityApp;
  m_probabilityApp = nullptr;
  delete m_calculationApp;
  m_calculationApp = nullptr;
  delete m_hardwareTestApp;
  m_hardwareTestApp = nullptr;
  delete m_regressionApp;
  m_regressionApp = nullptr;
  delete m_sequenceApp;
  m_sequenceApp = nullptr;
  delete m_settingsApp;
  m_settingsApp = nullptr;
  delete m_statisticsApp;
  m_statisticsApp = nullptr;
}

bool AppsContainer::poincareCircuitBreaker(const Poincare::Expression * e) {
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  return Ion::Keyboard::keyDown(Ion::Keyboard::Key::A6, state);
}

int AppsContainer::numberOfApps() {
  return k_numberOfApps;
}

App * AppsContainer::appAtIndex(int index) {
  App * apps[] = {
    m_homeApp,
    m_calculationApp,
    m_graphApp,
    m_sequenceApp,
    m_settingsApp,
    m_statisticsApp,
    m_probabilityApp,
    m_regressionApp,
  };
  assert(sizeof(apps)/sizeof(apps[0]) == k_numberOfApps);
  assert(index >= 0 && index < k_numberOfApps);
  return apps[index];
}

App * AppsContainer::hardwareTestApp() {
  return m_hardwareTestApp;
}

void AppsContainer::reset() {
  Clipboard::sharedClipboard()->reset();
  if (m_calculationApp != nullptr) {
    delete m_calculationApp;
  }
  m_calculationApp = new Calculation::App(this, &m_globalContext);
  if (m_graphApp != nullptr) {
    delete m_graphApp;
  }
  m_graphApp = new Graph::App(this, &m_globalContext);
  if (m_sequenceApp != nullptr) {
    delete m_sequenceApp;
  }
  m_sequenceApp = new Sequence::App(this, &m_globalContext);
  if (m_statisticsApp != nullptr) {
    delete m_statisticsApp;
  }
  m_statisticsApp = new Statistics::App(this);
  if (m_probabilityApp != nullptr) {
    delete m_probabilityApp;
  }
  m_probabilityApp = new Probability::App(this);
  if (m_regressionApp != nullptr) {
    delete m_regressionApp;
  }
  m_regressionApp = new Regression::App(this);
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
  if (event == Ion::Events::Home && activeApp() != m_hardwareTestApp) {
    switchTo(appAtIndex(0));
    return true;
  }
  if (event == Ion::Events::OnOff && activeApp() != m_hardwareTestApp) {
    suspend(true);
    return true;
  }
  bool didProcessEvent = Container::dispatchEvent(event);
  if (!didProcessEvent && event == Ion::Events::Back) {
    switchTo(appAtIndex(0));
    return true;
  }
  if (!didProcessEvent && alphaLockWantsRedraw) {
    window()->redraw();
    return true;
  }
  return didProcessEvent;
}

void AppsContainer::switchTo(App * app) {
  if (app == hardwareTestApp()) {
    m_window.hideTitleBarView(true);
  } else {
    m_window.hideTitleBarView(false);
  }
  if (app) {
    m_window.setTitle(app->upperName());
  }
  Container::switchTo(app);
}

void AppsContainer::updateBatteryState() {
  if (m_window.updateBatteryLevel() || m_window.updateIsChargingState() || m_window.updatePluggedState()) {
    m_window.redraw();
  }
}

void AppsContainer::refreshPreferences() {
  m_window.refreshPreferences();
}

void AppsContainer::displayExamModePopUp(bool activate, bool forceWindowRedraw) {
  m_examPopUpController.setActivatingExamMode(activate);
  activeApp()->displayModalViewController(&m_examPopUpController, 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpRightMargin, Metric::PopUpBottomMargin, Metric::PopUpLeftMargin);
  if (forceWindowRedraw) {
    m_window.redraw(true);
  }
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

Window * AppsContainer::window() {
  return &m_window;
}

int AppsContainer::numberOfTimers() {
  return 4+(GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate);
}

Timer * AppsContainer::timerAtIndex(int i) {
  switch (i) {
    case 0:
      return &m_batteryTimer;
    case 1:
      return &m_USBTimer;
    case 2:
      return &m_suspendTimer;
    case 3:
      return &m_backlightDimmingTimer;
    case 4:
      return &m_ledTimer;
    default:
      assert(false);
      return nullptr;
  }
}
