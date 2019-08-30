#include "values_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Sequence {

ValuesController::ValuesController(Responder * parentResponder,InputEventHandlerDelegate * inputEventHandlerDelegate, Interval * interval, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, inputEventHandlerDelegate, header, interval),
  m_sequenceTitleCells{},
  m_floatCells{},
#if COPY_COLUMN
  m_sequenceParameterController('n'),
#endif
  m_intervalParameterController(this, inputEventHandlerDelegate, m_interval)
{
  for (int i = 0; i < k_maxNumberOfSequences; i++) {
    m_sequenceTitleCells[i].setOrientation(FunctionTitleCell::Orientation::HorizontalIndicator);
  }
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::ValuesController::willDisplayCellAtLocation(cell, i, j);
  // The cell is the abscissa title cell:
  if (typeAtLocation(i,j) == 0) {
    EvenOddMessageTextCell * mytitleCell = (EvenOddMessageTextCell *)cell;
    mytitleCell->setMessage(I18n::Message::N);
    return;
  }
  // The cell is a function title cell:
  if (typeAtLocation(i,j) == 1) {
    SequenceTitleCell * myCell = (SequenceTitleCell *)cell;
    Sequence * sequence = functionStore()->modelForRecord(recordAtColumn(i));
    myCell->setLayout(sequence->nameLayout());
    myCell->setColor(sequence->color());
  }
}

I18n::Message ValuesController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
}

bool ValuesController::setDataAtLocation(double floatBody, int columnIndex, int rowIndex) {
  if (floatBody < 0) {
      return false;
  }
  return Shared::ValuesController::setDataAtLocation(std::round(floatBody), columnIndex, rowIndex);
}

int ValuesController::maxNumberOfCells() {
  return k_maxNumberOfCells;
}

int ValuesController::maxNumberOfFunctions() {
  return k_maxNumberOfSequences;
}

SequenceTitleCell * ValuesController::functionTitleCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfSequences);
  return &m_sequenceTitleCells[j];
}

EvenOddBufferTextCell * ValuesController::floatCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfCells);
  return &m_floatCells[j];
}

ViewController * ValuesController::functionParameterController() {
#if COPY_COLUMN
  m_sequenceParameterController.setRecord(recordAtColumn(selectedColumn()));
  return &m_sequenceParameterController;
#else
  return nullptr;
#endif
}

I18n::Message ValuesController::valuesParameterControllerPageTitle() const {
  return I18n::Message::NColumn;
}

}
