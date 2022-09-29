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
#include "store_menu_controller.h"
#include "suspend_timer.h"
#include "backlight_dimming_timer.h"
#include "shared/global_context.h"
#include "on_boarding/prompt_controller.h"
#include <escher/blink_timer.h>
#include <escher/container.h>
#include <poincare/preferences.h>
#include <ion/events.h>

class AppsContainer : public Escher::Container, ExamPopUpControllerDelegate, Ion::Storage::StorageDelegate {
public:
  static AppsContainer * sharedAppsContainer();
  AppsContainer();
  virtual int numberOfBuiltinApps() = 0;
  int numberOfApps() { return numberOfExternalApps() + numberOfBuiltinApps(); }
  virtual Escher::App::Snapshot * appSnapshotAtIndex(int index) = 0;
  Ion::ExternalApps::App externalAppAtIndex(int index);
  Escher::App::Snapshot * initialAppSnapshot();
  Escher::App::Snapshot * hardwareTestAppSnapshot();
  Escher::App::Snapshot * onBoardingAppSnapshot();
  Escher::App::Snapshot * usbConnectedAppSnapshot();
  Home::App::Snapshot * homeAppSnapshot() { return &m_homeSnapshot; }
  void reset();
  Shared::GlobalContext * globalContext();
  MathToolbox * mathToolbox();
  MathVariableBoxController * variableBoxController();
  void didSuspend(bool checkIfOnOffKeyReleased = false);
  bool dispatchEvent(Ion::Events::Event event) override;
  void switchToBuiltinApp(Escher::App::Snapshot * snapshot) override;
  void switchToExternalApp(Ion::ExternalApps::App app);
  void run() override;
  bool updateBatteryState();
  void refreshPreferences();
  void reloadTitleBarView();
  void displayExamModePopUp(Poincare::Preferences::ExamMode mode, Poincare::Preferences::PressToTestParams pressToTestParams = Poincare::Preferences::k_inactivePressToTest);
  void shutdownDueToLowBattery();
  void setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus newStatus);
  OnBoarding::PromptController * promptController();
  void redrawWindow();
  void activateExamMode(Poincare::Preferences::ExamMode examMode);
  // Exam pop-up controller delegate
  void examDeactivatingPopUpIsDismissed() override;
  // Ion::Storage::StorageDelegate
  void storageDidChangeForRecord(const Ion::Storage::Record record) override;
  void storageIsFull() override;
#if EPSILON_GETOPT
  void setInitialAppSnapshot(Escher::App::Snapshot * snapshot) { m_initialAppSnapshot = snapshot; }
#endif
protected:
  int numberOfExternalApps() { return Ion::ExternalApps::numberOfApps(); }
private:
  Escher::Window * window() override;
  int numberOfContainerTimers() override;
  Escher::Timer * containerTimerAtIndex(int i) override;
  bool processEvent(Ion::Events::Event event);
  void resetShiftAlphaStatus();
  bool updateAlphaLock();
  void handleRunException(bool resetSnapshot);

  static const I18n::Message k_promptMessages[];
  static const KDColor k_promptColors[];
  static const int k_promptNumberOfMessages;
  bool m_firstUSBEnumeration;
  AppsWindow m_window;
  EmptyBatteryWindow m_emptyBatteryWindow;
  Shared::GlobalContext m_globalContext;
  MathToolbox m_mathToolbox;
  MathVariableBoxController m_variableBoxController;
  StoreMenuController m_storeController;
  ExamPopUpController m_examPopUpController;
  OnBoarding::PromptController m_promptController;
  BatteryTimer m_batteryTimer;
  SuspendTimer m_suspendTimer;
  BacklightDimmingTimer m_backlightDimmingTimer;
  Escher::BlinkTimer m_blinkTimer;
  Home::App::Snapshot m_homeSnapshot;
  OnBoarding::App::Snapshot m_onBoardingSnapshot;
  HardwareTest::App::Snapshot m_hardwareTestSnapshot;
  USB::App::Snapshot m_usbConnectedSnapshot;
#if EPSILON_GETOPT
  // Used to launch a given app on a simulator
  Escher::App::Snapshot * m_initialAppSnapshot;
#endif
};

#endif
