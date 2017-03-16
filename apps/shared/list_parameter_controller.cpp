#include "list_parameter_controller.h"
#include <assert.h>

namespace Shared {

ListParameterController::ListParameterController(Responder * parentResponder, FunctionStore * functionStore) :
  ViewController(parentResponder),
  m_selectableTableView(SelectableTableView(this, this, 1, Metric::CommonTopMargin, Metric::CommonRightMargin,
    Metric::CommonBottomMargin, Metric::CommonLeftMargin)),
  m_functionStore(functionStore),
  m_colorCell(PointerTableCellWithChevron((char*)"Couleur de la fonction")),
  m_enableCell(PointerTableCellWithSwitch((char*)"Activer/Desactiver")),
  m_deleteCell(PointerTableCell((char*)"Supprimer la fonction"))
{
}

const char * ListParameterController::title() const {
  return "Options de la fonction";
}

View * ListParameterController::view() {
  return &m_selectableTableView;
}

void ListParameterController::didBecomeFirstResponder() {
  m_selectableTableView.reloadData();
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

void ListParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (cell == &m_enableCell) {
    SwitchView * switchView = (SwitchView *)m_enableCell.accessoryView();
    switchView->setState(m_function->isActive());
  }
}

void ListParameterController::setFunction(Function * function) {
  m_function = function;
  m_selectableTableView.selectCellAtLocation(0, 0);
}

bool ListParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    return handleEnterOnRow(m_selectableTableView.selectedRow());
  }
  return false;
}

int ListParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

HighlightCell * ListParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  HighlightCell * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
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
    case 0:
      return true;
    case 1:
      m_function->setActive(!m_function->isActive());
      m_selectableTableView.reloadData();
      return true;
    case 2:
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
        app()->displayWarning("Pas de fonction a supprimer");
        return false;
      }
  }
  default:
    return false;
  }
}

}
