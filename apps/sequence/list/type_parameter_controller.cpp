#include "type_parameter_controller.h"
#include <assert.h>

namespace Sequence {

TypeParameterController::TypeParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_expliciteCell(TextBufferMenuListCell((char*)"Explicite")),
  m_singleRecurrenceCell(TextBufferMenuListCell((char*)"Recurrence d'ordre 1")),
  m_doubleRecurenceCell(TextBufferMenuListCell((char*)"Recurrence d'ordre 2")),
  m_selectableTableView(SelectableTableView(this, this, Metric::TopMargin, Metric::RightMargin,
    Metric::BottomMargin, Metric::LeftMargin))
{
}

const char * TypeParameterController::title() const {
  return "Choisir le type de suite";
}

View * TypeParameterController::view() {
  return &m_selectableTableView;
}

void TypeParameterController::didBecomeFirstResponder() {
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

void TypeParameterController::setSequence(Sequence * sequence) {
  m_sequence = sequence;
  char buffer[8];
  buffer[0] = sequence->name()[0];
  strlcpy(buffer+1, "(n)=", 7);
  m_expliciteCell.setText(buffer);
  strlcpy(buffer+1, "(n+1)= ", 7);
  m_singleRecurrenceCell.setText(buffer);
  strlcpy(buffer+1, "(n+2)=", 7);
  m_doubleRecurenceCell.setText(buffer);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    m_sequence->setType((Sequence::Type)m_selectableTableView.selectedRow());
    StackViewController * stack = stackController();
    stack->pop();
    return true;
  }
  return false;
}

int TypeParameterController::numberOfRows() {
  return k_totalNumberOfCell;
};

TableViewCell * TypeParameterController::reusableCell(int index) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  TableViewCell * cells[] = {&m_expliciteCell, &m_singleRecurrenceCell, &m_doubleRecurenceCell};
  return cells[index];
}

int TypeParameterController::reusableCellCount() {
  return k_totalNumberOfCell;
}

KDCoordinate TypeParameterController::cellHeight() {
  return Metric::ParameterCellHeight;
}

StackViewController * TypeParameterController::stackController() const {
  return (StackViewController *)parentResponder();
}


}
