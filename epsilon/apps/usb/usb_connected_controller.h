#pragma once

#include <escher/message_view.h>
#include <escher/view_controller.h>

namespace USB {

class USBConnectedController : public Escher::ViewController {
 public:
  USBConnectedController();
  Escher::View* view() override { return &m_messageView; }
  bool handleEvent(Ion::Events::Event event) override { return false; }

 private:
  Escher::MessageView m_messageView;
};

}  // namespace USB
