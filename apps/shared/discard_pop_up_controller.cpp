#include "discard_pop_up_controller.h"
#include <apps/i18n.h>

using namespace Escher;

namespace Shared {

DiscardPopUpController::DiscardPopUpController(Invocation OkInvocation) :
  PopUpController(2, OkInvocation, I18n::Message::Warning, I18n::Message::Ok, I18n::Message::Cancel)
{
  m_contentView.setMessage(0, I18n::Message::ConfirmDiscard1);
  m_contentView.setMessage(1, I18n::Message::ConfirmDiscard2);
}

}
