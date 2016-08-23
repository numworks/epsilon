#ifndef APPS_CONTAINER_H
#define APPS_CONTAINER_H

#include "graph/graph_app.h"
#include "probability/app.h"

#define USE_PIC_VIEW_APP 0
#if USE_PIC_VIEW_APP
#include "picview/picview_app.h"
#endif

class AppsContainer : public Container {
public:
  AppsContainer();
  enum class AppId {
    Graph = 0,
    Probability = 1,
#if USE_PIC_VIEW_APP
    PicView = 2,
    Count = 3
#else
    Count = 2
#endif
  };
  void switchTo(AppId appId);
  bool handleEvent(Ion::Events::Event event) override;
private:
  App * appWithId(AppId appId);
  int m_activeAppIndex;
  GraphApp m_graphApp;
  Probability::App m_probabilityApp;
#if USE_PIC_VIEW_APP
  PicViewApp m_picViewApp;
#endif
};

#endif
