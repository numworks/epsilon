#include "values_function_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

namespace Shared {

const char * ValuesFunctionParameterController::title() {
  return m_pageTitle;
}

void ValuesFunctionParameterController::viewWillAppear() {
  FunctionApp * myApp = static_cast<FunctionApp *>(app());
  myApp->functionStore()->modelForRecord(m_record)->nameWithArgument(m_pageTitle, Function::k_maxNameWithArgumentSize, m_symbol);
}

void ValuesFunctionParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

}
