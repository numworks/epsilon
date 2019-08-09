#include "go_to_parameter_controller.h"
#include <assert.h>

namespace Shared {

GoToParameterController::GoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol) :
  FloatParameterController(parentResponder),
  m_cursor(cursor),
  m_graphRange(graphRange),
  m_abscisseCell(&m_selectableTableView, inputEventHandlerDelegate, this, symbol)
{
}

int GoToParameterController::numberOfRows() {
  return 2;
}

HighlightCell * GoToParameterController::reusableParameterCell(int index, int type) {
  assert(index == 0);
  return &m_abscisseCell;
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
