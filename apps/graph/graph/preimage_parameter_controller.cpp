#include "preimage_parameter_controller.h"
#include "../app.h"
#include <assert.h>

namespace Graph {

PreimageParameterController::PreimageParameterController(
  Responder * parentResponder,
  InputEventHandlerDelegate * inputEventHandlerDelegate,
  Shared::InteractiveCurveViewRange * graphRange,
  Shared::CurveViewCursor * cursor,
  PreimageGraphController * preimageGraphController
) :
  Shared::GoToParameterController(
    parentResponder,
    inputEventHandlerDelegate,
    graphRange,
    cursor
  ),
  m_preimageGraphController(preimageGraphController)
{
}

const char * PreimageParameterController::title() {
  return I18n::translate(I18n::Message::Preimage);
}

void PreimageParameterController::viewWillAppear() {
  setParameterName(I18n::Message::Y);
  m_preimageGraphController->setImage(m_cursor->y());
  Shared::GoToParameterController::viewWillAppear();
}

void PreimageParameterController::buttonAction() {
  m_preimageGraphController->setRecord(m_record);
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  stack->pop();
  stack->pop();
  stack->push(m_preimageGraphController);
}

double PreimageParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  return m_preimageGraphController->image();
}
bool PreimageParameterController::setParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex == 0);
  m_preimageGraphController->setImage(f);
  return true;
}

}
