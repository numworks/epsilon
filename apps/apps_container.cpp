#include "apps_container.h"
extern "C" {
#include <assert.h>
}

AppsContainer::AppsContainer() :
  Container(),
  m_homeApp(this),
  m_graphApp(&m_context),
  m_calculApp(&m_context),
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
    &m_calculApp,
  };
  assert(sizeof(apps)/sizeof(apps[0]) == k_numberOfApps);
  assert(index >= 0 && index < k_numberOfApps);
  return apps[index];
}

bool AppsContainer::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Event::F1) {
    switchTo(appAtIndex(0));
    return true;
  }
  return false;
}
