#include "parameter_controller.h"
#include <assert.h>

ParameterController::ParameterController(Responder * parentResponder, Graph::FunctionStore * functionStore) :
  ViewController(parentResponder),
  m_colorCell(TableViewCell((char*)"Couleur de la fonction")),
  m_enableCell(SwitchTableViewCell((char*)"Activer/Desactiver")),
  m_deleteCell(TableViewCell((char*)"Supprimer la fonction")),
  m_tableView(TableView(this)),
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
  setActiveCell(0);
}

void ParameterController::willDisplayCellForIndex(View * cell, int index) {
  if (cell == &m_enableCell) {
    SwitchView * switchView = (SwitchView *)m_enableCell.contentView();
    switchView->setState(m_function->isActive());
  }
}

void ParameterController::setActiveCell(int index) {
  if (index < 0 || index >= k_totalNumberOfCell) {
    return;
  }
  TableViewCell * previousCell = (TableViewCell *)(m_tableView.cellAtIndex(m_activeCell));
  previousCell->setHighlighted(false);

  m_activeCell = index;
  m_tableView.scrollToRow(index);
  TableViewCell * cell = (TableViewCell *)(m_tableView.cellAtIndex(index));
  cell->setHighlighted(true);

}


void ParameterController::setFunction(Graph::Function * function) {
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
      SwitchView * switchView = (SwitchView *)m_enableCell.contentView();
      switchView->setState(!switchView->state());
      m_function->setActive(!m_function->isActive());
      return true;
    }
    case 2:
    {
      m_functionStore->removeFunction(m_function);
      StackViewController * stack = (StackViewController *)(parentResponder());
      stack->pop();
      return true;
    }
    default:
    {
      return false;
    }
  }
}

int ParameterController::numberOfCells() {
  return k_totalNumberOfCell;
};


View * ParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  View * cells[] = {&m_colorCell, &m_enableCell, &m_deleteCell};
  return cells[index];
}

int ParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate ParameterController::cellHeight() {
  return 35;
}
