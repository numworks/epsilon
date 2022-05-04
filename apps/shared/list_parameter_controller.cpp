#include "list_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  SelectableListViewController(parentResponder, tableDelegate),
  m_enableCell(I18n::Message::ActivateDeactivate),
  m_deleteCell(deleteFunctionMessage)
{
}

const char * ListParameterController::title() {
  return I18n::translate(I18n::Message::FunctionOptions);
}

void ListParameterController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void ListParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  resetMemoization();
  m_selectableTableView.reloadData();
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_enableCell && !m_record.isNull()) {
    assert(typeAtIndex(index) == k_enableCellType);
    m_enableCell.setState(!function()->isActive());
  }
}

int ListParameterController::typeAtIndex(int index) {
  switch (sharedCellIndex(index)) {
  case 0:
    return k_enableCellType;
  default:
    assert(sharedCellIndex(index) == 1);
    return k_deleteCellType;
  }
}

void ListParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  selectCellAtLocation(0, 0);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return handleEnterOnRow(selectedRow());
  }
  return false;
}

int ListParameterController::sharedCellIndex(int j) {
  // Shared::ListParameterController rows are always placed last
  assert(j >= numberOfRows() - k_numberOfSharedCells && j < numberOfRows());
  return j - (numberOfRows() - k_numberOfSharedCells);
}

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  if (type == k_enableCellType) {
    return &m_enableCell;
  }
  assert(type == k_deleteCellType);
  return &m_deleteCell;
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  switch (typeAtIndex(rowIndex)) {
    case k_enableCellType:
      function()->setActive(!function()->isActive());
      resetMemoization();
      m_selectableTableView.reloadData();
      return true;
    case k_deleteCellType:
      {
        assert(functionStore()->numberOfModels() > 0);
        m_selectableTableView.deselectTable();
        functionStore()->removeModel(m_record);
        setRecord(Ion::Storage::Record());
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      }
    default:
      return false;
  }
}

ExpiringPointer<Function> ListParameterController::function() {
  return functionStore()->modelForRecord(m_record);
}

FunctionStore * ListParameterController::functionStore() {
  return FunctionApp::app()->functionStore();
}


}
