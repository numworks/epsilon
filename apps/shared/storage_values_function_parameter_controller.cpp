#include "storage_values_function_parameter_controller.h"
#include "storage_function_app.h"
#include <assert.h>

namespace Shared {

const char * StorageValuesFunctionParameterController::title() {
  StorageFunctionApp * myApp = static_cast<StorageFunctionApp *>(app());
  myApp->functionStore()->modelForRecord(m_record)->nameWithArgument(m_pageTitle, StorageFunction::k_maxNameWithArgumentSize, m_symbol);
  return m_pageTitle;
}

void StorageValuesFunctionParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

}
