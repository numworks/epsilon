#include "goto_parameter_controller.h"
#include <assert.h>

namespace Graph {

GoToParameterController::GoToParameterController(Responder * parentResponder, GraphView * graphView) :
  FloatParameterController(parentResponder),
  m_abscisseCell(TextMenuListCell((char*)"x")),
  m_graphView(graphView),
  m_function(nullptr)
{
}

const char * GoToParameterController::title() const {
  return "Aller a";
}

float GoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  return m_graphView->xCursorPosition();
}

void GoToParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 0);
  m_graphView->setXCursorPosition(f, m_function);
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
}
