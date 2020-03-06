#include "list_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, tableDelegate),
  m_record(),
  m_colorParameterController(this),
  m_colorCell(functionColorMessage),
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
  m_selectableTableView.reloadData();
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_enableCell) {
    SwitchView * switchView = (SwitchView *)m_enableCell.accessoryView();
    switchView->setState(function()->isActive());
  }
}

void ListParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  selectCellAtLocation(0, 0);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  int selectedR = selectedRow();
  if (event == Ion::Events::OK || event == Ion::Events::EXE || (event == Ion::Events::Right && selectedR == 3)) {
    return handleEnterOnRow(selectedR);
  }
  return false;
}

KDCoordinate ListParameterController::cumulatedHeightFromIndex(int j) {
  return Metric::ParameterCellHeight * j;
}

int ListParameterController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return (offsetY - 1) / Metric::ParameterCellHeight;
}

HighlightCell * ListParameterController::reusableCell(int index, int type) {
  assert(index == 0);
  assert(index < totalNumberOfCells());
  HighlightCell * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
  return cells[type];
}

int ListParameterController::typeAtLocation(int i, int j) {
  return j;
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  StackViewController * stack = (StackViewController *)(parentResponder());
  switch (rowIndex) {
    case 0:
      m_colorParameterController.setRecord(m_record);
      stack->push(&m_colorParameterController);
      return true;
    case 1:
      function()->setActive(!function()->isActive());
      m_selectableTableView.reloadData();
      return true;
    case 2:
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
