#include "empty_python_variable_box.h"

#include <apps/i18n.h>

using namespace Escher;

namespace Code {

EmptyPythonVariableBox::VariableBoxEmptyView::VariableBoxEmptyView()
    : ::ModalViewEmptyController::ModalViewEmptyView() {
  initMessageViews();
  m_message.setMessage(I18n::Message::NoWordAvailableHere);
}

}  // namespace Code
