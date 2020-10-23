#ifndef USB_APP_H
#define USB_APP_H

#include "escher/include/escher/app.h"
#include "usb_connected_controller.h"
#include "../shared/shared_app.h"

namespace USB {

class App : public Escher::App {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    App * unpack(Escher::Container * container) override;
    Descriptor * descriptor() override;
  };
  bool processEvent(Ion::Events::Event) override;
private:
  App(Snapshot * snapshot);
  USBConnectedController m_usbConnectedController;
};

}

#endif
