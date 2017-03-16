#include "values_controller.h"
#include <assert.h>

using namespace Shared;

namespace Sequence {

ValuesController::ValuesController(Responder * parentResponder, SequenceStore * sequenceStore, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header, I18n::Message::NColumn),
  m_sequenceTitleCells{SequenceTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator), SequenceTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator),
    SequenceTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator)},
  m_sequenceStore(sequenceStore),
  m_sequenceParameterController(Shared::ValuesFunctionParameterController('n'))
{
}

int ValuesController::numberOfColumns() {
  return m_sequenceStore->numberOfActiveFunctions()+1;
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::ValuesController::willDisplayCellAtLocation(cell, i, j);
  // The cell is the abscissa title cell:
  if (j == 0 && i == 0) {
    EvenOddPointerTextCell * mytitleCell = (EvenOddPointerTextCell *)cell;
    mytitleCell->setMessage(I18n::Message::N);
    return;
  }
  // The cell is a function title cell:
  if (j == 0 && i > 0) {
    SequenceTitleCell * myCell = (SequenceTitleCell *)cell;
    Sequence * sequence = m_sequenceStore->activeFunctionAtIndex(i-1);
    myCell->setExpression(sequence->nameLayout());
    myCell->setColor(sequence->color());
  }
}

I18n::Message ValuesController::emptyMessage() {
  if (m_sequenceStore->numberOfDefinedFunctions() == 0) {
    return I18n::Message::NoSequence;
  }
  return I18n::Message::NoActivatedSequence;
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

SequenceStore * ValuesController::functionStore() const {
  return m_sequenceStore;
}

Shared::ValuesFunctionParameterController * ValuesController::functionParameterController() {
  return &m_sequenceParameterController;
}

}
