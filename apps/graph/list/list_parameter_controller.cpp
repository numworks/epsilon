#include "list_parameter_controller.h"
#include <assert.h>

using namespace Shared;

namespace Graph {

HighlightCell * ListParameterController::reusableCell(int index) {
  if (index == 0) {
    return &m_renameCell;
  }
  return StorageListParameterController::reusableCell(index -1);
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  if (rowIndex == 0) {
    return true;
  }
  return StorageListParameterController::handleEnterOnRow(rowIndex-1);
}

}
