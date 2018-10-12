#include "storage_values_function_parameter_controller.h"
#include <assert.h>

namespace Shared {

const char * StorageValuesFunctionParameterController::title() {
  m_function->nameWithArgument(m_pageTitle, k_maxNumberOfCharsInTitle, m_symbol);
  return m_pageTitle;
}

void StorageValuesFunctionParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

}
