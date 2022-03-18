#include "function_curve_parameter_controller.h"
#include "interactive_curve_view_controller.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

FunctionCurveParameterController::FunctionCurveParameterController() :
  SelectableListViewController(nullptr),
  m_goToCell(I18n::Message::Goto),
  m_record()
{
}

void FunctionCurveParameterController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

bool FunctionCurveParameterController::handleEvent(Ion::Events::Event event) {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  if (event == Ion::Events::Left
   && stack->depth() > Shared::InteractiveCurveViewController::k_graphControllerStackDepth + 1)
  {
    /* We only allow popping with Left if there is another menu beneath this
     * one. */
    stack->pop();
    return true;
  }
  return false;
}

bool FunctionCurveParameterController::handleGotoSelection() {
  if (m_record.isNull()) {
    return false;
  }
  goToParameterController()->setRecord(m_record);
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->push(goToParameterController());
  return true;
}

}
