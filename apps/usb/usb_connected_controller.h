#ifndef USB_USB_CONNECTED_CONTROLLER_H
#define USB_USB_CONNECTED_CONTROLLER_H

#include <escher.h>
#include "../shared/message_view.h"

namespace USB {

class USBConnectedController : public ViewController {
public:
  USBConnectedController();
  View * view() override { return &m_messageView; }
  bool handleEvent(Ion::Events::Event event) override { return false; }
private:
  MessageView m_messageView;
};

}

#endif

