#include "go_to_parameter_controller.h"
#include "interactive_curve_view_controller.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

GoToParameterController::GoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  FloatParameterController<double>(parentResponder),
  m_cursor(cursor),
  m_graphRange(graphRange),
  m_parameterCell(&m_selectableTableView, inputEventHandlerDelegate, this)
{
}

HighlightCell * GoToParameterController::reusableParameterCell(int index, int type) {
  assert(index == 0);
  return &m_parameterCell;
}

bool GoToParameterController::handleEvent(Ion::Events::Event & event) {
  if (event == Ion::Events::Left) {
    stackController()->pop();
    return true;
  }
  return false;
}

void GoToParameterController::viewWillAppear() {
  // Initialize m_tempParameter to the extracted value.
  setParameterAtIndex(0, extractParameterAtIndex(0));
  FloatParameterController::viewWillAppear();
}

bool GoToParameterController::setParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex == 0);
  m_tempParameter = f;
  return true;
}

void GoToParameterController::buttonAction() {
  // Update parameter value to m_tempParameter, and proceed if value is valid
  if (confirmParameterAtIndex(0, m_tempParameter)) {
    StackViewController * stack = (StackViewController *)parentResponder();
    stack->popUntilDepth(InteractiveCurveViewController::k_graphControllerStackDepth, true);
  }
}

}
