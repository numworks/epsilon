#pragma once

#include <escher/app.h>
#include <escher/bank_view_controller.h>

#include "persisting_bytes_viewer_controller.h"
#include "storage_viewer_controller.h"

namespace HardwareCheckup {

class App : public Escher::App {
 public:
  class Snapshot : public Escher::App::Snapshot {
   public:
    App* unpack(Escher::Container* container) override;
    const Descriptor* descriptor() const override;
  };

 private:
  class WizardViewController : public Escher::BankViewController {
   public:
    WizardViewController(Escher::Responder* parentResponder);
    int numberOfChildren() override { return k_numberOfControllers; }
    Escher::ViewController* childAtIndex(int i) override;
    bool handleEvent(Ion::Events::Event event) override;

   private:
    static constexpr int k_numberOfControllers = 2;
    StorageViewerController m_storageViewerController;
    PersistingBytesViewerController m_persistingBytesViewerController;
    // Col3Controller m_col3Controller;
    // ExternalAppsController m_externalAppsController;
    // StandByController m_standByController;
    // ScreenController m_screenController;
    // PressedKeyController m_pressedKeyController;
    // OTPController m_otpController;
  };

  App(Snapshot* snapshot);
  WizardViewController m_wizardViewController;
};

}  // namespace HardwareCheckup
