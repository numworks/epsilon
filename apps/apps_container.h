#ifndef APPS_CONTAINER_H
#define APPS_CONTAINER_H

#include <escher/blink_timer.h>
#include <escher/container.h>
#include <ion/events.h>
#include <poincare/preferences.h>

#include "apps_window.h"
#include "backlight_dimming_timer.h"
#include "battery_timer.h"
#include "empty_battery_window.h"
#include "exam_pop_up_controller.h"
#include "hardware_test/app.h"
#include "home/app.h"
#include "on_boarding/app.h"
#include "on_boarding/prompt_controller.h"
#include "shared/global_context.h"
#include "suspend_timer.h"
#include "usb/app.h"

class AppsContainer : public Escher::Container, Ion::Storage::StorageDelegate {
 public:
  static AppsContainer* sharedAppsContainer();
  AppsContainer();
  virtual int numberOfBuiltinApps() = 0;
  int numberOfApps() { return numberOfExternalApps() + numberOfBuiltinApps(); }
  int numberOfExternalApps() { return Ion::ExternalApps::numberOfApps(); }
  virtual Escher::App::Snapshot* appSnapshotAtIndex(int index) = 0;
  Ion::ExternalApps::App externalAppAtIndex(int index);
  Escher::App::Snapshot* initialAppSnapshot();
  Escher::App::Snapshot* hardwareTestAppSnapshot();
  Escher::App::Snapshot* onBoardingAppSnapshot();
  Escher::App::Snapshot* usbConnectedAppSnapshot();
  Home::App::Snapshot* homeAppSnapshot() { return &m_homeSnapshot; }
  void setExamMode(Poincare::ExamMode targetExamMode,
                   Poincare::ExamMode previousMode);
  Shared::GlobalContext* globalContext();
  void didSuspend();
  bool dispatchEvent(Ion::Events::Event event) override;
  void switchToBuiltinApp(Escher::App::Snapshot* snapshot) override;
  void switchToExternalApp(Ion::ExternalApps::App app);
  void run() override;
  bool updateBatteryState();
  void refreshPreferences();
  void reloadTitleBarView();
  void displayExamModePopUp(Poincare::ExamMode mode);
  void shutdownDueToLowBattery();
  void setShiftAlphaStatus(Ion::Events::ShiftAlphaStatus newStatus);
  OnBoarding::PromptController* promptController();
  void redrawWindow();
  void setDFUBetweenEvents(bool active) { m_dfuBetweenEvents = active; }

  // Ion::Storage::StorageDelegate
  bool storageCanChangeForRecordName(
      const Ion::Storage::Record::Name recordName) const override;
  void storageDidChangeForRecord(const Ion::Storage::Record record) override;
  void storageIsFull() override;

#if EPSILON_GETOPT
  void setInitialAppSnapshot(Escher::App::Snapshot* snapshot) {
    m_initialAppSnapshot = snapshot;
  }
#endif

 private:
  Escher::Window* window() override;
  int numberOfContainerTimers() override;
  Escher::Timer* containerTimerAtIndex(int i) override;
  void listenToExternalEvents() override;
  bool processEvent(Ion::Events::Event event);
  void resetShiftAlphaStatus();
  bool updateAlphaLock();
  void handleRunException();
  void openDFU(bool blocking);
  static void ShowCursor();

  static const I18n::Message k_promptMessages[];
  static const KDColor k_promptColors[];
  static const int k_promptNumberOfMessages;
  bool m_firstUSBEnumeration;
  bool m_dfuBetweenEvents;
  AppsWindow m_window;
  EmptyBatteryWindow m_emptyBatteryWindow;
  Shared::GlobalContext m_globalContext;
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
  Escher::App::Snapshot* m_initialAppSnapshot;
#endif
};

#endif
