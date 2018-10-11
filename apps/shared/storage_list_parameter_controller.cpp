#include "storage_list_parameter_controller.h"
#include <assert.h>

namespace Shared {

StorageListParameterController::StorageListParameterController(Responder * parentResponder, StorageFunctionStore * functionStore, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, this, tableDelegate),
  m_functionStore(functionStore),
  m_function(nullptr),
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

View * StorageListParameterController::view() {
  return &m_selectableTableView;
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
    switchView->setState(m_function->isActive());
  }
}

void StorageListParameterController::setFunction(StorageFunction * function) {
  m_function = function;
  selectCellAtLocation(0, 0);
}

bool StorageListParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return handleEnterOnRow(selectedRow());
  }
  return false;
}

int StorageListParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * StorageListParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
#if FUNCTION_COLOR_CHOICE
  HighlightCell * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
#else
  HighlightCell * cells[] = {&m_enableCell, &m_deleteCell};
#endif
  return cells[index];
}

int StorageListParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate StorageListParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
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
      m_function->setActive(!m_function->isActive());
      m_selectableTableView.reloadData();
      return true;
#if FUNCTION_COLOR_CHOICE
      case 2:
#else
      case 1:
#endif
    {
      if (m_functionStore->numberOfModels() > 1) {
        m_functionStore->removeModel(m_function);
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      } else {
        if (m_functionStore->numberOfDefinedModels() == 1) {
          StorageFunction * f = m_functionStore->definedModelAtIndex(0);
          f->setContent("");
          StackViewController * stack = (StackViewController *)(parentResponder());
          stack->pop();
          return true;
        }
        app()->displayWarning(I18n::Message::NoFunctionToDelete);
        return true;
      }
  }
  default:
    return false;
  }
}

}
