#ifndef USB_APP_H
#define USB_APP_H

#include "escher/include/escher/app.h"
#include "usb_connected_controller.h"
#include "../shared/shared_app.h"

namespace USB {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
  };
  class Snapshot : public ::SharedApp::Snapshot {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
  };
  bool processEvent(Ion::Events::Event) override;
private:
  App(Snapshot * snapshot);
  USBConnectedController m_usbConnectedController;
};

}

#endif
