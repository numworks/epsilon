#include "list_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

using namespace Escher;

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, tableDelegate),
  m_record(),
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
  if (cell == &m_enableCell) {
    SwitchView * switchView = (SwitchView *)m_enableCell.accessoryView();
    if (!m_record.isNull()) {
      switchView->setState(function()->isActive());
    }
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

KDCoordinate ListParameterController::nonMemoizedRowHeight(int j) {
  switch (j) {
    case 0:
      prepareCellForHeightCalculation((HighlightCell *)&m_enableCell, j);
      return m_enableCell.minimalSizeForOptimalDisplay().height();
    case 1:
      prepareCellForHeightCalculation((HighlightCell *)&m_deleteCell, j);
      return m_deleteCell.minimalSizeForOptimalDisplay().height();
    default:
      assert(false);
      return KDCoordinate(0);
  }
}

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  assert(index == 0);
  assert(index < totalNumberOfCells());
  HighlightCell * cells[] = {&m_enableCell, &m_deleteCell};
  return cells[type];
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  switch (rowIndex) {
    case 0:
      function()->setActive(!function()->isActive());
      resetMemoization();
      m_selectableTableView.reloadData();
      return true;
    case 1:
      {
        assert(functionStore()->numberOfModels() > 0);
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
