#include "list_parameter_controller.h"
#include <assert.h>

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, FunctionStore * functionStore, I18n::Message functionColorMessage, I18n::Message deleteFunctionMessage, SelectableTableViewDelegate * tableDelegate) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin, this, tableDelegate),
  m_functionStore(functionStore),
  m_function(nullptr),
#if FUNCTION_COLOR_CHOICE
  m_colorCell(functionColorMessage),
#endif
  m_enableCell(I18n::Message::ActivateDesactivate),
  m_deleteCell(deleteFunctionMessage)
{
}

const char * ListParameterController::title() {
  return I18n::translate(I18n::Message::FunctionOptions);
}

View * ListParameterController::view() {
  return &m_selectableTableView;
}

void ListParameterController::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_selectableTableView);
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
    switchView->setState(m_function->isActive());
  }
}

void ListParameterController::setFunction(Function * function) {
  m_function = function;
  selectCellAtLocation(0, 0);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return handleEnterOnRow(selectedRow());
  }
  return false;
}

int ListParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * ListParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
#if FUNCTION_COLOR_CHOICE
  HighlightCell * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
#else
  HighlightCell * cells[] = {&m_enableCell, &m_deleteCell};
#endif
  return cells[index];
}

int ListParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ListParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
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
      m_function->setActive(!m_function->isActive());
      m_selectableTableView.reloadData();
      return true;
#if FUNCTION_COLOR_CHOICE
      case 2:
#else
      case 1:
#endif
    {
      if (m_functionStore->numberOfFunctions() > 1) {
        m_functionStore->removeFunction(m_function);
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      } else {
        if (m_functionStore->numberOfDefinedFunctions() == 1) {
          Function * f = m_functionStore->definedFunctionAtIndex(0);
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
