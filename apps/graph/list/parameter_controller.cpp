#include "parameter_controller.h"
#include <assert.h>

namespace Graph {

ParameterController::ParameterController(Responder * parentResponder, FunctionStore * functionStore) :
  ViewController(parentResponder),
  m_colorCell(ListViewCell((char*)"Couleur de la fonction")),
  m_enableCell(SwitchListViewCell((char*)"Activer/Desactiver")),
  m_deleteCell(ListViewCell((char*)"Supprimer la fonction")),
  m_tableView(TableView(this,Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin)),
  m_activeCell(0),
  m_functionStore(functionStore)
{
}

const char * ParameterController::title() const {
  return "Options de la fonction";
}

View * ParameterController::view() {
  return &m_tableView;
}

void ParameterController::didBecomeFirstResponder() {
  m_tableView.reloadData();
  setActiveCell(0);
}

void ParameterController::willDisplayCellForIndex(TableViewCell * cell, int index) {
  if (cell == &m_enableCell) {
    SwitchView * switchView = (SwitchView *)m_enableCell.contentView();
    switchView->setState(m_function->isActive());
  }
}

void ParameterController::setActiveCell(int index) {
  if (index < 0 || index >= k_totalNumberOfCell) {
    return;
  }
  ListViewCell * previousCell = (ListViewCell *)(m_tableView.cellAtLocation(0, m_activeCell));
  previousCell->setHighlighted(false);

  m_activeCell = index;
  m_tableView.scrollToCell(0, index);
  ListViewCell * cell = (ListViewCell *)(m_tableView.cellAtLocation(0, index));
  cell->setHighlighted(true);

}


void ParameterController::setFunction(Function * function) {
  m_function = function;
}

bool ParameterController::handleEvent(Ion::Events::Event event) {
  switch (event) {
    case Ion::Events::Event::DOWN_ARROW:
      setActiveCell(m_activeCell+1);
      return true;
    case Ion::Events::Event::UP_ARROW:
      setActiveCell(m_activeCell-1);
      return true;
    case Ion::Events::Event::ENTER:
      return handleEnter();
    default:
      return false;
  }
}

bool ParameterController::handleEnter() {
  switch (m_activeCell) {
    case 0:
    {
      return true;
    }
    case 1:
    {
      m_function->setActive(!m_function->isActive());
      m_tableView.reloadData();
      return true;
    }
    case 2:
    {
      if (m_functionStore->numberOfFunctions() > 1) {
        m_functionStore->removeFunction(m_function);
        StackViewController * stack = (StackViewController *)(parentResponder());
        stack->pop();
        return true;
      } else {
        // TODO: add an warning "no function to delete!"
        return false;
      }
    }
    default:
    {
      return false;
    }
  }
}

int ParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};


TableViewCell * ParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
  return cells[index];
}

int ParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ParameterController::cellHeight() {
  return 35;
}

}
