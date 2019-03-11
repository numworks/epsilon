#include "storage_list_parameter_controller.h"
#include "function_app.h"
#include <assert.h>

namespace Shared {

StorageListParameterController::StorageListParameterController(Responder * parentResponder, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
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

const char * StorageListParameterController::title() {
  return I18n::translate(I18n::Message::FunctionOptions);
}

void StorageListParameterController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
}

void StorageListParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  m_selectableTableView.reloadData();
}

void StorageListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_enableCell) {
    SwitchView * switchView = (SwitchView *)m_enableCell.accessoryView();
    switchView->setState(function()->isActive());
  }
}

void StorageListParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  selectCellAtLocation(0, 0);
}

bool StorageListParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return handleEnterOnRow(selectedRow());
  }
  return false;
}

HighlightCell * StorageListParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < totalNumberOfCells());
#if FUNCTION_COLOR_CHOICE
  HighlightCell * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
#else
  HighlightCell * cells[] = {&m_enableCell, &m_deleteCell};
#endif
  return cells[index];
}

bool StorageListParameterController::handleEnterOnRow(int rowIndex) {
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
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      }
      default:
        return false;
  }
}

ExpiringPointer<Function> StorageListParameterController::function() {
  return functionStore()->modelForRecord(m_record);
}

FunctionStore * StorageListParameterController::functionStore() {
  FunctionApp * a = static_cast<FunctionApp *>(app());
  return a->functionStore();
}


}
