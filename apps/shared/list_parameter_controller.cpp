#include "list_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, tableDelegate),
  m_record(),
  m_colorCell(),
  m_enableCell(I18n::Message::ActivateDeactivate),
  m_deleteCell(deleteFunctionMessage),
  m_colorParameterController(parentResponder, functionColorMessage)
{
  m_colorCell.setMessage(functionColorMessage);
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
  } else if(cell == &m_colorCell) {
    int index = -1;
    KDColor color = function()->color();
    for(int i = 0; i < Palette::numberOfDataColors(); i++) {
      if(color == Palette::DataColor[i]) {
        index = i;
      }
    }
    assert(index >= 0);
    m_colorCell.setSubtitle(MessageTableCellWithColor::k_textForIndex[index]);
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
  switch (rowIndex) {
    case 0: {
      StackViewController * stack = (StackViewController *)(parentResponder());
      m_colorParameterController.setRecord(m_record);
      stack->push(&m_colorParameterController);
      return true;
    }
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
