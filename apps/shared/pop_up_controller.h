#ifndef SHARED_POP_UP_CONTROLLER_H
#define SHARED_POP_UP_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/pop_up_controller.h>

namespace Shared {

// These classes exist only to have default constructors with default messages.
// They can't be in Escher because I18n messages are compiled after Escher is.

class MessagePopUpController : public Escher::MessagePopUpController {
 public:
  MessagePopUpController(Escher::Invocation OkInvocation,
                         I18n::Message message = I18n::Message::ConfirmDiscard)
      : Escher::MessagePopUpController(OkInvocation, I18n::Message::Warning,
                                       I18n::Message::Ok,
                                       I18n::Message::Cancel) {
    setContentMessage(message);
  }
};

class BufferPopUpController : public Escher::BufferPopUpController {
 public:
  BufferPopUpController(Escher::Invocation OkInvocation)
      : Escher::BufferPopUpController(OkInvocation, I18n::Message::Warning,
                                      I18n::Message::Ok,
                                      I18n::Message::Cancel) {}
};

}  // namespace Shared

#endif
