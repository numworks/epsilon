#include "go_to_parameter_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>

namespace Shared {

GoToParameterController::GoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol) :
  FloatParameterController(parentResponder),
  m_abscisseCell(MessageTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, symbol)),
  m_graphRange(graphRange),
  m_cursor(cursor),
  m_function(nullptr)
{
}

const char * GoToParameterController::title() {
  return I18n::translate(I18n::Message::Goto);
}

void GoToParameterController::viewWillAppear() {
  m_previousParameter = parameterAtIndex(0);
  FloatParameterController::viewWillAppear();
}

int GoToParameterController::numberOfRows() {
  return 2;
}

float GoToParameterController::previousParameterAtIndex(int index) {
  assert(index == 0);
  return m_previousParameter;
}

float GoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  return m_cursor->x();
}

bool GoToParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 0);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = m_function->evaluateAtAbscissa(f, myApp->localContext());
  if (fabsf(f) > k_maxDisplayableFloat || fabsf(y) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  m_cursor->moveTo(f, y);
  return true;
}

HighlightCell * GoToParameterController::reusableParameterCell(int index, int type) {
  assert(index == 0);
  return &m_abscisseCell;
}

int GoToParameterController::reusableParameterCellCount(int type) {
  return 1;
}

void GoToParameterController::setFunction(Function * function) {
  m_function = function;
}

void GoToParameterController::buttonAction() {
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->pop();
  stack->pop();
}

}
