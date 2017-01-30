#include "apps_container.h"
#include <ion.h>
extern "C" {
#include <assert.h>
}

AppsContainer::AppsContainer() :
  Container(),
  m_window(AppsWindow()),
  m_homeApp(this),
  m_graphApp(this, &m_globalContext),
  m_probabilityApp(this),
  m_calculationApp(this, &m_globalContext),
  m_regressionApp(this),
  m_settingsApp(this, &m_preferences),
  m_statisticsApp(this),
  m_globalContext(GlobalContext()),
  m_preferences(Preferences()),
  m_variableBoxController(&m_globalContext)
{
}

int AppsContainer::numberOfApps() {
  return k_numberOfApps;
}

App * AppsContainer::appAtIndex(int index) {
  static App * apps[] = {
    &m_homeApp,
    &m_calculationApp,
    &m_graphApp,
    &m_graphApp,
    &m_graphApp,
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

ToolboxController * AppsContainer::toolboxController() {
  return &m_toolboxController;
}

VariableBoxController * AppsContainer::variableBoxController() {
  return &m_variableBoxController;
}

bool AppsContainer::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Home) {
    switchTo(appAtIndex(0));
    return true;
  }
  if (event == Ion::Events::OnOff) {
    Ion::Power::suspend();
  }
  return false;
}

void AppsContainer::switchTo(App * app) {
  m_window.setTitle(app->upperName());
  // TODO: Update the battery icon every in a time frame
  m_window.updateBatteryLevel();
  Container::switchTo(app);
}

Window * AppsContainer::window() {
  return &m_window;
}
