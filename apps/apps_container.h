#ifndef APPS_CONTAINER_H
#define APPS_CONTAINER_H

#include "home/app.h"
#include "graph/app.h"
#include "probability/app.h"
#include "calculation/app.h"
#include "tool_box_controller.h"

#define USE_PIC_VIEW_APP 0
#if USE_PIC_VIEW_APP
#include "picview/picview_app.h"
#endif

class AppsContainer : public Container {
public:
  AppsContainer();

  int numberOfApps();
  App * appAtIndex(int index);
  ToolBoxController * toolBoxController();
  bool handleEvent(Ion::Events::Event event) override;
private:
  static constexpr int k_numberOfApps = 4;
  Home::App m_homeApp;
  Graph::App m_graphApp;
  Probability::App m_probabilityApp;
  Calculation::App m_calculationApp;
#if USE_PIC_VIEW_APP
  PicViewApp m_picViewApp;
#endif
  Context m_context;
  ToolBoxController m_toolBoxController;
};

#endif
