#include "go_to_parameter_controller.h"
#include <assert.h>

namespace Shared {

GoToParameterController::GoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  FloatParameterController<double>(parentResponder),
  m_cursor(cursor),
  m_graphRange(graphRange),
  m_parameterCell(&m_selectableTableView, inputEventHandlerDelegate, this)
{
}

int GoToParameterController::numberOfRows() const {
  return 2;
}

HighlightCell * GoToParameterController::reusableParameterCell(int index, int type) {
  assert(index == 0);
  return &m_parameterCell;
}

int GoToParameterController::reusableParameterCellCount(int type) {
  return 1;
}

bool GoToParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

void GoToParameterController::buttonAction() {
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->pop();
  stack->pop();
}

}
