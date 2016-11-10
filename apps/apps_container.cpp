#include "apps_container.h"
extern "C" {
#include <assert.h>
}

AppsContainer::AppsContainer() :
  Container(),
  m_homeApp(this),
  m_graphApp(this, &m_context),
  m_probabilityApp(this),
  m_calculationApp(this, &m_context),
  m_context(Context())
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
  };
  assert(sizeof(apps)/sizeof(apps[0]) == k_numberOfApps);
  assert(index >= 0 && index < k_numberOfApps);
  return apps[index];
}

Context * AppsContainer::context() {
  return &m_context;
}

ToolboxController * AppsContainer::toolboxController() {
  return &m_toolboxController;
}

VariableBoxController * AppsContainer::variableBoxController() {
  return &m_variableBoxController;
}

bool AppsContainer::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Event::F1) {
    switchTo(appAtIndex(0));
    return true;
  }
  return false;
}
