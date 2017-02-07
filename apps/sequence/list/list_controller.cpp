#include "list_controller.h"
#include <assert.h>

using namespace Shared;

namespace Sequence {

ListController::ListController(Responder * parentResponder, SequenceStore * sequenceStore, HeaderViewController * header) :
  Shared::ListController(parentResponder, sequenceStore, header),
  m_parameterController(ListParameterController(this, sequenceStore))
{
}

const char * ListController::title() const {
  return "Suites";
}

bool ListController::handleEvent(Ion::Events::Event event) {
  if (Shared::ListController::handleEvent(event)) {
    return true;
  }
  if (event == Ion::Events::OK && m_selectableTableView.selectedColumn() == 1
      && m_selectableTableView.selectedRow() == numberOfRows() - 1) {
    return addFunction();
  }
  return false;
}

void ListController::tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) {
  if (t->selectedRow() < numberOfRows() - 1) {
    Responder * myCell = (Responder *)t->cellAtLocation(t->selectedColumn(), t->selectedRow());
    app()->setFirstResponder(myCell);
  } else {
    app()->setFirstResponder(t);
  }
}

ListParameterController * ListController::parameterController() {
  return &m_parameterController;
}

int ListController::maxNumberOfRows() {
  return k_maxNumberOfRows;
}

TableViewCell * ListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_functionTitleCells[index];
}

TableViewCell * ListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfRows);
  return &m_expressionCells[index];
}

}
