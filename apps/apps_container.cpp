#include "apps_container.h"
extern "C" {
#include <assert.h>
}

AppsContainer::AppsContainer() :
  Container(),
  m_homeApp(this),
  m_graphApp(this, &m_globalContext),
  m_probabilityApp(this, &m_globalContext),
  m_calculationApp(this, &m_globalContext),
  m_statisticsApp(this, &m_globalContext),
  m_globalContext(GlobalContext()),
  m_variableBoxController(&m_globalContext)
{
}

int AppsContainer::numberOfApps() {
  return k_numberOfApps;
}

App * AppsContainer::appAtIndex(int index) {
  static App * apps[] = {
    &m_homeApp,
    &m_graphApp,
    &m_probabilityApp,
    &m_calculationApp,
    &m_statisticsApp
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
  return false;
}
