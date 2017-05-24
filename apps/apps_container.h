#ifndef APPS_CONTAINER_H
#define APPS_CONTAINER_H

#include "home/app.h"
#include "graph/app.h"
#include "probability/app.h"
#include "calculation/app.h"
#include "regression/app.h"
#include "sequence/app.h"
#include "settings/app.h"
#include "statistics/app.h"
#include "on_boarding/app.h"
#include "hardware_test/app.h"
#include "on_boarding/update_controller.h"
#include "apps_window.h"
#include "empty_battery_window.h"
#include "math_toolbox.h"
#include "variable_box_controller.h"
#include "exam_pop_up_controller.h"
#include "led_timer.h"
#include "battery_timer.h"
#include "usb_timer.h"
#include "suspend_timer.h"
#include "backlight_dimming_timer.h"

#define USE_PIC_VIEW_APP 0
#if USE_PIC_VIEW_APP
#include "picview/picview_app.h"
#endif

class AppsContainer : public Container {
public:
  AppsContainer();
  static bool poincareCircuitBreaker(const Poincare::Expression * e);
  int numberOfApps();
  App::Snapshot * appSnapshotAtIndex(int index);
  App::Snapshot * hardwareTestAppSnapshot();
  App::Snapshot * onBoardingAppSnapshot();
  void reset();
  Poincare::Context * globalContext();
  MathToolbox * mathToolbox();
  VariableBoxController * variableBoxController();
  void suspend(bool checkIfPowerKeyReleased = false);
  virtual bool dispatchEvent(Ion::Events::Event event) override;
  void switchTo(App::Snapshot * snapshot) override;
  void run() override;
  bool updateBatteryState();
  void refreshPreferences();
  void displayExamModePopUp(bool activate);
  void shutdownDueToLowBattery();
  void reloadTitleBar();
  UpdateController * updatePopUpController();
private:
  Window * window() override;
  int numberOfTimers() override;
  Timer * timerAtIndex(int i) override;
  static constexpr int k_numberOfCommonApps = 8;
  static constexpr int k_totalNumberOfApps = 2+k_numberOfCommonApps;
  AppsWindow m_window;
  EmptyBatteryWindow m_emptyBatteryWindow;
#if USE_PIC_VIEW_APP
  PicViewApp m_picViewApp;
#endif
  Poincare::GlobalContext m_globalContext;
  MathToolbox m_mathToolbox;
  VariableBoxController m_variableBoxController;
  ExamPopUpController m_examPopUpController;
  UpdateController m_updateController;
  LedTimer m_ledTimer;
  BatteryTimer m_batteryTimer;
  USBTimer m_USBTimer;
  SuspendTimer m_suspendTimer;
  BacklightDimmingTimer m_backlightDimmingTimer;
  HardwareTest::App::Snapshot m_hardwareTestSnapshot;
  OnBoarding::App::Snapshot m_onBoardingSnapshot;
  Home::App::Snapshot m_homeSnapshot;
  Calculation::App::Snapshot m_calculationSnapshot;
  Graph::App::Snapshot m_graphSnapshot;
  Sequence::App::Snapshot m_sequenceSnapshot;
  Settings::App::Snapshot m_settingsSnapshot;
  Statistics::App::Snapshot m_statisticsSnapshot;
  Probability::App::Snapshot m_probabilitySnapshot;
  Regression::App::Snapshot m_regressionSnapshot;
};

#endif
