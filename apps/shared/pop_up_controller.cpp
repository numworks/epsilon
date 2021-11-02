#include "pop_up_controller.h"

namespace Shared {

PopUpController::PopUpController(Escher::Invocation OkInvocation, std::initializer_list<I18n::Message> messages) : Escher::PopUpController(messages.size(), OkInvocation, I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel) {
  int index = 0;
  for (I18n::Message message : messages) {
    m_contentView.setMessage(index++, message);
  }
}

}
