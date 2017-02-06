#ifndef APPS_CONTAINER_H
#define APPS_CONTAINER_H

#include "home/app.h"
#include "graph/app.h"
#include "probability/app.h"
#include "calculation/app.h"
#include "regression/app.h"
#include "settings/app.h"
#include "statistics/app.h"
#include "apps_window.h"
#include "toolbox_controller.h"
#include "variable_box_controller.h"
#include "preferences.h"

#define USE_PIC_VIEW_APP 0
#if USE_PIC_VIEW_APP
#include "picview/picview_app.h"
#endif

class AppsContainer : public Container {
public:
  AppsContainer();

  int numberOfApps();
  App * appAtIndex(int index);
  Poincare::Context * globalContext();
  Preferences * preferences();
  ToolboxController * toolboxController();
  VariableBoxController * variableBoxController();
  bool handleEvent(Ion::Events::Event event) override;
  void switchTo(App * app) override;
  void refreshPreferences();
private:
  Window * window() override;
  static constexpr int k_numberOfApps = 9;
  AppsWindow m_window;
  Home::App m_homeApp;
  Graph::App m_graphApp;
  Probability::App m_probabilityApp;
  Calculation::App m_calculationApp;
  Regression::App m_regressionApp;
  Settings::App m_settingsApp;
  Statistics::App m_statisticsApp;
#if USE_PIC_VIEW_APP
  PicViewApp m_picViewApp;
#endif
  Poincare::GlobalContext m_globalContext;
  Preferences m_preferences;
  ToolboxController m_toolboxController;
  VariableBoxController m_variableBoxController;
};

#endif
