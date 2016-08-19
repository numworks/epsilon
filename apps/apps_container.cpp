#include "apps_container.h"

AppsContainer::AppsContainer() :
  Container(),
  m_activeAppIndex(0)
{
}

bool AppsContainer::handleEvent(ion_event_t event) {
  if (event == F1) {
    m_activeAppIndex++;
    if (m_activeAppIndex >= (int)(AppId::Count)) {
      m_activeAppIndex = 0;
    }
    switchTo((AppId)m_activeAppIndex);
    return true;
  }
  return false;
}

void AppsContainer::switchTo(AppId appId) {
  Container::switchTo(appWithId(appId));
}

App * AppsContainer::appWithId(AppId appId) {
  App * apps[] = {
    &m_graphApp,
    &m_probabilityApp,
#if USE_PIC_VIEW_APP
    &m_picViewApp,
#endif
  };
  return apps[(int)appId];
};
