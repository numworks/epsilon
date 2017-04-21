#ifndef APPS_CONTAINER_H
#define APPS_CONTAINER_H

#include "home/app.h"
#include "graph/app.h"
#include "probability/app.h"
#include "calculation/app.h"
#include "hardware_test/app.h"
#include "regression/app.h"
#include "sequence/app.h"
#include "settings/app.h"
#include "statistics/app.h"
#include "apps_window.h"
#include "empty_battery_window.h"
#include "math_toolbox.h"
#include "variable_box_controller.h"
#include "exam_pop_up_controller.h"
#include "led_timer.h"
#include "battery_timer.h"
#include "usb_timer.h"

#define USE_PIC_VIEW_APP 0
#if USE_PIC_VIEW_APP
#include "picview/picview_app.h"
#endif

class AppsContainer : public Container {
public:
  AppsContainer();

  int numberOfApps();
  App * appAtIndex(int index);
  App * hardwareTestApp();
  void reset();
  Poincare::Context * globalContext();
  MathToolbox * mathToolbox();
  VariableBoxController * variableBoxController();
  virtual bool dispatchEvent(Ion::Events::Event event) override;
  void switchTo(App * app) override;
  void updateBatteryState();
  void refreshPreferences();
  void displayExamModePopUp(bool activate, bool forceRedrawWindow);
  void shutdownDueToLowBattery();
private:
  Window * window() override;
  int numberOfTimers() override;
  Timer * timerAtIndex(int i) override;
  static constexpr int k_numberOfApps = 8;
  AppsWindow m_window;
  EmptyBatteryWindow m_emptyBatteryWindow;
  Home::App m_homeApp;
  Graph::App m_graphApp;
  Probability::App m_probabilityApp;
  Calculation::App m_calculationApp;
  HardwareTest::App m_hardwareTestApp;
  Regression::App m_regressionApp;
  Sequence::App m_sequenceApp;
  Settings::App m_settingsApp;
  Statistics::App m_statisticsApp;
#if USE_PIC_VIEW_APP
  PicViewApp m_picViewApp;
#endif
  Poincare::GlobalContext m_globalContext;
  MathToolbox m_mathToolbox;
  VariableBoxController m_variableBoxController;
  ExamPopUpController m_examPopUpController;
  LedTimer m_ledTimer;
  BatteryTimer m_batteryTimer;
  USBTimer m_USBTimer;
};

#endif
