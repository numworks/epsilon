#include "list_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, tableDelegate),
  m_record(),
#if FUNCTION_COLOR_CHOICE
  m_colorCell(functionColorMessage),
#endif
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
#if FUNCTION_COLOR_CHOICE
  HighlightCell * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
#else
  HighlightCell * cells[] = {&m_enableCell, &m_deleteCell};
#endif
  return cells[type];
}

int ListParameterController::typeAtLocation(int i, int j) {
  return j;
}

bool ListParameterController::handleEnterOnRow(int rowIndex) {
  switch (rowIndex) {
#if FUNCTION_COLOR_CHOICE
    case 0:
    /* TODO: implement function color choice */
      return true;
    case 1:
#else
    case 0:
#endif
      function()->setActive(!function()->isActive());
      m_selectableTableView.reloadData();
      return true;
#if FUNCTION_COLOR_CHOICE
      case 2:
#else
      case 1:
#endif
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
