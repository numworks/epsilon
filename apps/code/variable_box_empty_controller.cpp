#include "variable_box_empty_controller.h"
#include <apps/i18n.h>

namespace Code {

VariableBoxEmptyController::VariableBoxEmptyView::VariableBoxEmptyView() :
  ::VariableBoxEmptyController::VariableBoxEmptyView()
{
  initMessageViews();
  m_messages[0].setMessage(I18n::Message::Degrees); //TODO LEA
  m_messages[1].setMessage(I18n::Message::Degrees);
}

}
