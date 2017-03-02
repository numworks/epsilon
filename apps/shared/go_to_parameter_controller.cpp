#include "go_to_parameter_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>

namespace Shared {

GoToParameterController::GoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, const char * symbol) :
  FloatParameterController(parentResponder),
  m_abscisseCell(PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, (char*)symbol)),
  m_graphRange(graphRange),
  m_cursor(cursor),
  m_function(nullptr)
{
}

const char * GoToParameterController::title() const {
  return "Aller a";
}

float GoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  return m_cursor->x();
}

void GoToParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 0);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = m_function->evaluateAtAbscissa(f, myApp->localContext());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, f);
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, y);
  m_cursor->moveTo(f, y);
}

int GoToParameterController::numberOfRows() {
  return 1;
};

HighlightCell * GoToParameterController::reusableCell(int index) {
  assert(index == 0);
  return &m_abscisseCell;
}

int GoToParameterController::reusableCellCount() {
  return 1;
}

void GoToParameterController::setFunction(Function * function) {
  m_function = function;
}

bool GoToParameterController::textFieldDidFinishEditing(TextField * textField, const char * text) {
  FloatParameterController::textFieldDidFinishEditing(textField, text);
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->pop();
  stack->pop();
  return true;
}

}
