#ifndef SHARED_POP_UP_CONTROLLER_H
#define SHARED_POP_UP_CONTROLLER_H

#include <escher/pop_up_controller.h>
#include <apps/i18n.h>

namespace Shared {

// These classes exist only to have default constructors with default messages.
// They can't be in Escher because I18n messages are compiled after Escher is.

class MessagePopUpController : public Escher::MessagePopUpController {
public:
  MessagePopUpController(Escher::Invocation OkInvocation, std::initializer_list<I18n::Message> messages = {I18n::Message::ConfirmDiscard1, I18n::Message::ConfirmDiscard2}) :
    Escher::MessagePopUpController(messages.size(), OkInvocation, I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel)
    {
      int index = 0;
      for (I18n::Message message : messages) {
        setContentMessage(index++, message);
      }
    }
};

class BufferPopUpController : public Escher::BufferPopUpController {
public:
  BufferPopUpController(Escher::Invocation OkInvocation, int numberOfLines) :
    Escher::BufferPopUpController(numberOfLines, OkInvocation, I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel)
    {}
};

}

#endif
