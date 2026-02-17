#pragma once

#include <escher/app.h>
#include <escher/bank_view_controller.h>

#include "clearance_viewer_controller.h"
#include "standby_controller.h"
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
    static constexpr int k_numberOfControllers = 3;
    StandbyController m_standbyController;
    StorageViewerController m_storageViewerController;
    ClearanceViewerController m_clearanceViewerController;
  };

  App(Snapshot* snapshot);
  WizardViewController m_wizardViewController;
};

}  // namespace HardwareCheckup
