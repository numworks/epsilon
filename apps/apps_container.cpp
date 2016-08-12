#include "apps_container.h"

AppsContainer::AppsContainer() :
  Container()
{
}

bool AppsContainer::handleEvent(ion_event_t event) {
  if (event == F1) {
#if USE_PIC_VIEW_APP
    if (activeApp() == &m_picViewApp) {
      Container::switchTo(&m_graphApp);
    } else {
      Container::switchTo(&m_picViewApp);
    }
#endif
    return true;
  }
  return false;
}

void AppsContainer::switchTo(AppId appId) {
  Container::switchTo(appWithId(appId));
}

App * AppsContainer::appWithId(AppId appId) {
  App * apps[] = {
    &m_graphApp
#if USE_PIC_VIEW_APP
    , &m_picViewApp
#endif
  };
  return apps[(int)appId];
};
