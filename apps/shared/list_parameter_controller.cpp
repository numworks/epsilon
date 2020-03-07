#include "list_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, tableDelegate),
  m_record(),
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
  ((ColorView *)m_colorCell.accessoryView())->setColor(0);
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
  return handleEventOnRow(selectedR, event);
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

bool ListParameterController::handleEventOnRow(int rowIndex, Ion::Events::Event event) {
  StackViewController * stack = (StackViewController *)(parentResponder());
  switch (rowIndex) {
    case 0:
    {
      ColorView * colorView = (ColorView *)m_colorCell.accessoryView();
      if (event == Ion::Events::Right) {
        colorView->setColor(colorView->colorIndex() + 1);
        return true;
      }
      if (event == Ion::Events::Left) {
        colorView->setColor(colorView->colorIndex() - 1);
        return true;
      }
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        function()->setColor(colorView->color());
	stack->pop();
        return true;
      }
      return false;
    }
    case 1:
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        function()->setActive(!function()->isActive());
        m_selectableTableView.reloadData();
        return true;
      }
      return false;
    case 2:
      if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        assert(functionStore()->numberOfModels() > 0);
        functionStore()->removeModel(m_record);
        setRecord(Ion::Storage::Record());
        stack->pop();
        return true;
      }
      return false;
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
