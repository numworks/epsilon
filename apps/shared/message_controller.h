#ifndef SHARED_MESSAGE_CONTROLLER_H
#define SHARED_MESSAGE_CONTROLLER_H

#include <escher.h>
#include "message_view_with_skip.h"

class MessageController : public ViewController {
public:
  MessageController(I18n::Message * messages, KDColor * colors, uint8_t numberOfMessages);
  View * view() override { return &m_messageViewWithSkip; }
  bool handleEvent(Ion::Events::Event event) override;
private:
  MessageViewWithSkip m_messageViewWithSkip;
};

#endif

