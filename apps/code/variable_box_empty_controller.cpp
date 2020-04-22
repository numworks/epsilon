#include "variable_box_empty_controller.h"
#include <apps/i18n.h>

namespace Code {

VariableBoxEmptyController::VariableBoxEmptyView::VariableBoxEmptyView() :
  ::VariableBoxEmptyController::VariableBoxEmptyView()
{
  initMessageViews();
  m_message.setMessage(I18n::Message::NoWordAvailableHere);
}

}
