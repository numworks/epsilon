#ifndef USB_USB_CONNECTED_CONTROLLER_H
#define USB_USB_CONNECTED_CONTROLLER_H

#include <escher/view_controller.h>
#include "../shared/message_view.h"

namespace USB {

class USBConnectedController : public Escher::ViewController {
public:
  USBConnectedController();
  Escher::View * view() override { return &m_messageView; }
  bool handleEvent(Ion::Events::Event event) override { return false; }
private:
  Shared::MessageView m_messageView;
};

}

#endif

