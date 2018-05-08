#include "message_controller.h"
#include "../apps_container.h"

MessageController::MessageController(I18n::Message * messages, KDColor * colors, uint8_t numberOfMessages) :
  ViewController(nullptr),
  m_messageViewWithSkip(messages, colors, numberOfMessages)
{
}

bool MessageController::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Back && event != Ion::Events::OnOff && event != Ion::Events::Home) {
    app()->dismissModalViewController();
    return true;
  }
  return false;
}
