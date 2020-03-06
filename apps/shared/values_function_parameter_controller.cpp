#include "values_function_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

namespace Shared {

const char * ValuesFunctionParameterController::title() {
  return m_pageTitle;
}

void ValuesFunctionParameterController::viewWillAppear() {
  FunctionApp::app()->functionStore()->modelForRecord(m_record)->nameWithArgument(m_pageTitle, Function::k_maxNameWithArgumentSize);
}

void ValuesFunctionParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  selectCellAtLocation(0, 0);
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

}
