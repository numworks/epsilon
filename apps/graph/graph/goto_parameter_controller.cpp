#include "goto_parameter_controller.h"
#include "../app.h"
#include "../../apps_container.h"
#include <assert.h>

namespace Graph {

GoToParameterController::GoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor) :
  FloatParameterController(parentResponder),
  m_abscisseCell(EditableTextMenuListCell(&m_selectableTableView, this, m_draftTextBuffer, (char*)"x")),
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
  App * graphApp = (Graph::App *)app();
  AppsContainer * container = (AppsContainer *)graphApp->container();
  float y = m_function->evaluateAtAbscissa(f, graphApp->localContext(), container->preferences()->angleUnit());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, f);
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, y);
  m_cursor->moveTo(f, y);
}

int GoToParameterController::numberOfRows() {
  return 1;
};

TableViewCell * GoToParameterController::reusableCell(int index) {
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
