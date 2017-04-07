#include "apps_container.h"
#include "global_preferences.h"
#include <ion.h>

extern "C" {
#include <assert.h>
}

using namespace Poincare;

AppsContainer::AppsContainer() :
  Container(),
  m_window(AppsWindow()),
  m_homeApp(this),
  m_graphApp(this, &m_globalContext),
  m_probabilityApp(this),
  m_calculationApp(this, &m_globalContext),
  m_hardwareTestApp(this),
  m_regressionApp(this),
  m_sequenceApp(this, &m_globalContext),
  m_settingsApp(this),
  m_statisticsApp(this),
  m_globalContext(GlobalContext()),
  m_variableBoxController(&m_globalContext),
  m_examPopUpController(ExamPopUpController()),
  m_ledTimer(LedTimer()),
  m_batteryTimer(BatteryTimer(this)),
  m_examModeTimer(ExamModeTimer(this))
{
  refreshPreferences();
}

int AppsContainer::numberOfApps() {
  return k_numberOfApps;
}

App * AppsContainer::appAtIndex(int index) {
  static App * apps[] = {
    &m_homeApp,
    &m_calculationApp,
    &m_graphApp,
    &m_sequenceApp,
    &m_hardwareTestApp,
    &m_statisticsApp,
    &m_probabilityApp,
    &m_regressionApp,
    &m_settingsApp,
  };
  assert(sizeof(apps)/sizeof(apps[0]) == k_numberOfApps);
  assert(index >= 0 && index < k_numberOfApps);
  return apps[index];
}

Context * AppsContainer::globalContext() {
  return &m_globalContext;
}

MathToolbox * AppsContainer::mathToolbox() {
  return &m_mathToolbox;
}

VariableBoxController * AppsContainer::variableBoxController() {
  return &m_variableBoxController;
}

bool AppsContainer::dispatchEvent(Ion::Events::Event event) {
  // Home and Power buttons are not sent to apps. We handle them straight away.
  if (event == Ion::Events::Home) {
    switchTo(appAtIndex(0));
    return true;
  }
  if (event == Ion::Events::OnOff) {
    Ion::Power::suspend();
    /* Ion::Power::suspend() completely shuts down the LCD controller. Therefore
     * the frame memory is lost. That's why we need to force a window redraw
     * upon wakeup, otherwise the screen is filled with noise. */
    window()->redraw(true);
    return true;
  }
  bool didProcessEvent = Container::dispatchEvent(event);
  if (!didProcessEvent && event == Ion::Events::Back) {
    switchTo(appAtIndex(0));
    return true;
  }
  return didProcessEvent;
}

void AppsContainer::switchTo(App * app) {
  m_window.setTitle(app->upperName());
  Container::switchTo(app);
}

void AppsContainer::updateBatteryState() {
  if (m_window.updateBatteryLevel() || m_window.updateIsChargingState()) {
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

Window * AppsContainer::window() {
  return &m_window;
}

int AppsContainer::numberOfTimers() {
  return 2+(GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate);
}

Timer * AppsContainer::timerAtIndex(int i) {
  switch (i) {
    case 0:
      return &m_batteryTimer;
    case 1:
      return &m_examModeTimer;
    case 2:
      return &m_ledTimer;
    default:
      assert(false);
      return nullptr;
  }
}
