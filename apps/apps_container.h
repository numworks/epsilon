#ifndef APPS_CONTAINER_H
#define APPS_CONTAINER_H

#include "home/app.h"
#include "on_boarding/app.h"
#include "hardware_test/app.h"
#include "usb/app.h"
#include "apps_window.h"
#include "empty_battery_window.h"
#include "math_toolbox.h"
#include "math_variable_box_controller.h"
#include "exam_pop_up_controller.h"
#include "exam_pop_up_controller_delegate.h"
#include "battery_timer.h"
#include "suspend_timer.h"
#include "global_preferences.h"
#include "backlight_dimming_timer.h"
#include "shared/global_context.h"
#include "clock_timer.h"
#include "on_boarding/prompt_controller.h"
#include "xnt_loop.h"

#include <ion/events.h>

class AppsContainer : public Container, ExamPopUpControllerDelegate, Ion::StorageDelegate {
public:
  static AppsContainer * sharedAppsContainer();
  AppsContainer();
  static bool poincareCircuitBreaker();
  virtual int numberOfApps() = 0;
  virtual App::Snapshot * appSnapshotAtIndex(int index) = 0;
  virtual int appIndexFromSnapshot(App::Snapshot * snapshot) = 0;
  App::Snapshot * initialAppSnapshot();
  App::Snapshot * hardwareTestAppSnapshot();
  App::Snapshot * onBoardingAppSnapshot();
  App::Snapshot * usbConnectedAppSnapshot();
  void reset();
  Poincare::Context * globalContext();
  MathToolbox * mathToolbox();
  MathVariableBoxController * variableBoxController();
  void suspend(bool checkIfOnOffKeyReleased = false);
  bool dispatchEvent(Ion::Events::Event event) override;
  bool switchTo(App::Snapshot * snapshot) override;
  void run() override;
  bool updateClock();
  bool updateBatteryState();
  void refreshPreferences();
  void reloadTitleBarView();
  void displayExamModePopUp(GlobalPreferences::ExamMode mode);
  void shutdownDueToLowBattery();
  void setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus newStatus);
  CodePoint XNT(CodePoint defaultXNT, bool * shouldRemoveLastCharacter) { return m_XNTLoop.XNT(defaultXNT, shouldRemoveLastCharacter); }
  void resetXNT() { m_XNTLoop.reset(); }
  OnBoarding::PromptController * promptController();
  void redrawWindow(bool force = false);
  void activateExamMode(GlobalPreferences::ExamMode examMode);
  // Exam pop-up controller delegate
  void examDeactivatingPopUpIsDismissed() override;
  // Ion::StorageDelegate
  void storageDidChangeForRecord(const Ion::Storage::Record record) override;
  void storageIsFull() override;
  #ifdef EPSILON_GETOPT
  void setStartApp(App::Snapshot * snapshot) { m_startAppSnapshot = snapshot; }
  #endif
protected:
  Home::App::Snapshot * homeAppSnapshot() { return &m_homeSnapshot; }
private:
  Window * window() override;
  bool processEvent(Ion::Events::Event event);
  void resetShiftAlphaStatus();
  bool updateAlphaLock();

  static I18n::Message k_promptMessages[];
  static KDColor k_promptFGColors[];
  static KDColor k_promptBGColors[];
  static int k_promptNumberOfMessages;
  int m_currentAppIndex; // Home isn't included after the second app switching
  int m_lastAppIndex;
  AppsWindow m_window;
  EmptyBatteryWindow m_emptyBatteryWindow;
  Shared::GlobalContext m_globalContext;
  MathToolbox m_mathToolbox;
  MathVariableBoxController m_variableBoxController;
  ExamPopUpController m_examPopUpController;
  OnBoarding::PromptController m_promptController;
  BatteryTimer m_batteryTimer;
  SuspendTimer m_suspendTimer;
  BacklightDimmingTimer m_backlightDimmingTimer;
  ClockTimer m_clockTimer;
  Home::App::Snapshot m_homeSnapshot;
  OnBoarding::App::Snapshot m_onBoardingSnapshot;
  HardwareTest::App::Snapshot m_hardwareTestSnapshot;
  USB::App::Snapshot m_usbConnectedSnapshot;
  XNTLoop m_XNTLoop;
  #ifdef EPSILON_GETOPT
  App::Snapshot * m_startAppSnapshot;
  #endif
};

#endif
