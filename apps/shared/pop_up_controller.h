#ifndef SHARED_POP_UP_CONTROLLER_H
#define SHARED_POP_UP_CONTROLLER_H

#include <initializer_list>
#include <apps/i18n.h>
#include <escher/pop_up_controller.h>

namespace Shared {

class PopUpController : public Escher::MessagePopUpController {
public:
  PopUpController(Escher::Invocation OkInvocation, std::initializer_list<I18n::Message> messages = {I18n::Message::ConfirmDiscard1, I18n::Message::ConfirmDiscard2});
  void presentModally();
};

}

#endif
