#include "list_parameter_controller.h"
#include "list_controller.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  if (type == 0) {
    return &m_renameCell;
  }
  return Shared::ListParameterController::reusableCell(index, type - 1);
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  if (rowIndex == 0) {
    renameFunction();
    return true;
  }
  return Shared::ListParameterController::handleEnterOnRow(rowIndex-1);
}

void ListParameterController::renameFunction() {
  // Set editing true on function title
  StackViewController * stack = (StackViewController *)(parentResponder());
  stack->pop();
  m_listController->renameSelectedFunction();
}

}
