#include "list_parameter_controller.h"
#include "list_controller.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

HighlightCell * ListParameterController::reusableCell(int index) {
  if (index == 0) {
    return &m_renameCell;
  }
  return ListParameterController::reusableCell(index -1);
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  if (rowIndex == 0) {
    renameFunction();
    return true;
  }
  return ListParameterController::handleEnterOnRow(rowIndex-1);
}

void ListParameterController::renameFunction() {
  // Set editing true on function title
  StackViewController * stack = (StackViewController *)(parentResponder());
  stack->pop();
  m_listController->renameSelectedFunction();
}

}
