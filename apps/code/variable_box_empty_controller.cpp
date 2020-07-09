#include "variable_box_empty_controller.h"
#include <apps/i18n.h>

namespace Code {

VariableBoxEmptyController::VariableBoxEmptyView::VariableBoxEmptyView() :
  ::ModalViewEmptyController::ModalViewEmptyView()
{
  initMessageViews();
  m_message.setMessage(I18n::Message::NoWordAvailableHere);
}

}
