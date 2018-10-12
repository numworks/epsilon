#include "storage_values_function_parameter_controller.h"
#include <assert.h>

namespace Shared {

const char * StorageValuesFunctionParameterController::title() {
  strlcpy(m_pageTitle, I18n::translate(I18n::Message::FunctionColumn), k_maxNumberOfCharsInTitle);
  for (int currentChar = 0; currentChar < k_maxNumberOfCharsInTitle-1; currentChar++) {
    if (m_pageTitle[currentChar] == '(') {
      m_pageTitle[currentChar-1] = *m_function->fullName();
      m_pageTitle[currentChar+1] = m_symbol;
      break;
    }
  }
  return m_pageTitle;
}

void StorageValuesFunctionParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

}
