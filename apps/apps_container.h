#ifndef APPS_CONTAINER_H
#define APPS_CONTAINER_H

#include "graph/graph_app.h"
#define USE_PIC_VIEW_APP 0
#if USE_PIC_VIEW_APP
#include "picview/picview_app.h"
#endif

class AppsContainer : public Container {
public:
  AppsContainer();
  enum class AppId {
    Graph = 0,
    PicView = 1
  };
  void switchTo(AppId appId);
  bool handleEvent(ion_event_t event) override;
private:
  App * appWithId(AppId appId);
  GraphApp m_graphApp;
#if USE_PIC_VIEW_APP
  PicViewApp m_picViewApp;
#endif
};

#endif
