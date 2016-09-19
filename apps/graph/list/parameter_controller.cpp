#include "parameter_controller.h"
#include <assert.h>


static const char * sMessages[] = {
  "Couleur de la fonction",
  "Activer/Desactiver",
  "Supprimer la fonction"
};

ParameterController::ParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_tableView(TableView(this)),
  m_activeCell(0)
{
  m_messages = sMessages;
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

void ParameterController::setActiveCell(int index) {
  if (index < 0 || index >= k_totalNumberOfCell) {
    return;
  }
  TableViewCell * previousCell = (TableViewCell *)(m_tableView.cellAtIndex(m_activeCell));
  previousCell->setFocused(false);

  m_activeCell = index;
  m_tableView.scrollToRow(index);
  TableViewCell * cell = (TableViewCell *)(m_tableView.cellAtIndex(index));
  cell->setFocused(true);

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
      switch (m_activeCell) {
      case 0:
        return true;
      case 1:
        m_function->setActive(!m_function->isActive());
        return true;
      case 2:
        return true;
      }
    default:
      return false;
  }
}

int ParameterController::numberOfCells() {
  return k_totalNumberOfCell;
};


View * ParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  return &m_cells[index];
}

int ParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

void ParameterController::willDisplayCellForIndex(View * cell, int index) {
  TableViewCell * myCell = (TableViewCell *)cell;
  myCell->setMessage(m_messages[index]);
}

KDCoordinate ParameterController::cellHeight() {
  return 35;
}
