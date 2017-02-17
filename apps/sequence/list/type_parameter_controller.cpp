#include "type_parameter_controller.h"
#include <assert.h>

namespace Sequence {

TypeParameterController::TypeParameterController(Responder * parentResponder, SequenceStore * sequenceStore) :
  ViewController(parentResponder),
  m_expliciteCell(TextBufferMenuListCell((char*)"Explicite")),
  m_singleRecurrenceCell(TextBufferMenuListCell((char*)"Recurrence d'ordre 1")),
  m_doubleRecurenceCell(TextBufferMenuListCell((char*)"Recurrence d'ordre 2")),
  m_selectableTableView(SelectableTableView(this, this)),
  m_sequenceStore(sequenceStore)
{
}

const char * TypeParameterController::title() const {
  return "Choisir le type de suite";
}

View * TypeParameterController::view() {
  return &m_selectableTableView;
}

void TypeParameterController::didBecomeFirstResponder() {
  m_selectableTableView.dataHasChanged(true);
  m_selectableTableView.selectCellAtLocation(0, 0);
  app()->setFirstResponder(&m_selectableTableView);
}

bool TypeParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    Sequence * newSequence = m_sequenceStore->addEmptyFunction();
    newSequence->setType((Sequence::Type)m_selectableTableView.selectedRow());
    app()->dismissModalViewController();
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

void TypeParameterController::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  char buffer[8];
  char nextName = m_sequenceStore->firstAvailableName()[0];
  buffer[0] = nextName;
  if (j == 0) {
    strlcpy(buffer+1, "(n)=", 7);
  }
  if (j == 1) {
    strlcpy(buffer+1, "(n+1)=", 7);
  }
  if (j == 2) {
    strlcpy(buffer+1, "(n+2)=", 7);
  }
  TextBufferMenuListCell * myCell = (TextBufferMenuListCell *)cell;
  myCell->setText(buffer);
}

StackViewController * TypeParameterController::stackController() const {
  return (StackViewController *)parentResponder();
}


}
