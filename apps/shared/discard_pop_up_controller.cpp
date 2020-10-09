#include "discard_pop_up_controller.h"

namespace Shared {

DiscardPopUpController::DiscardPopUpController(Invocation OkInvocation) :
  PopUpController(2, OkInvocation)
{
  m_contentView.setMessage(0, I18n::Message::ConfirmDiscard1);
  m_contentView.setMessage(1, I18n::Message::ConfirmDiscard2);
}

}
