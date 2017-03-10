#include "go_to_parameter_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>

namespace Shared {

GoToParameterController::GoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, const char * symbol) :
  FloatParameterController(parentResponder, "Valider"),
  m_abscisseCell(PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, (char*)symbol)),
  m_graphRange(graphRange),
  m_cursor(cursor),
  m_function(nullptr)
{
}

const char * GoToParameterController::title() const {
  return "Aller a";
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

void GoToParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 0);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = m_function->evaluateAtAbscissa(f, myApp->localContext());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, f);
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, y);
  m_cursor->moveTo(f, y);
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
  StackViewController * stack = (StackViewController *)parentResponder();
  stack->pop();
  stack->pop();
}

}
