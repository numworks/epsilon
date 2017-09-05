#include "values_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Sequence {

ValuesController::ValuesController(Responder * parentResponder, SequenceStore * sequenceStore, Interval * interval, ButtonRowController * header) :
  Shared::ValuesController(parentResponder, header, I18n::Message::NColumn, &m_intervalParameterController, interval),
  m_sequenceTitleCells{},
  m_floatCells{},
  m_sequenceStore(sequenceStore),
#if COPY_COLUMN
  m_sequenceParameterController('n'),
#endif
  m_intervalParameterController(this, m_interval)
{
}

void ValuesController::willDisplayCellAtLocation(HighlightCell * cell, int i, int j) {
  Shared::ValuesController::willDisplayCellAtLocation(cell, i, j);
  // The cell is the abscissa title cell:
  if (j == 0 && i == 0) {
    EvenOddMessageTextCell * mytitleCell = (EvenOddMessageTextCell *)cell;
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

IntervalParameterController * ValuesController::intervalParameterController() {
  return &m_intervalParameterController;
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
  return m_sequenceTitleCells[j];
}

EvenOddBufferTextCell * ValuesController::floatCells(int j) {
  assert(j >= 0 && j < k_maxNumberOfCells);
  return m_floatCells[j];
}

SequenceStore * ValuesController::functionStore() const {
  return m_sequenceStore;
}

Shared::ValuesFunctionParameterController * ValuesController::functionParameterController() {
#if COPY_COLUMN
  return &m_sequenceParameterController;
#else
  return nullptr;
#endif
}

View * ValuesController::loadView() {
  for (int i = 0; i < k_maxNumberOfSequences; i++) {
    m_sequenceTitleCells[i] = new SequenceTitleCell(FunctionTitleCell::Orientation::HorizontalIndicator);
  }
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_floatCells[i] = new EvenOddBufferTextCell();
  }
  return Shared::ValuesController::loadView();
}

void ValuesController::unloadView(View * view) {
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    delete m_floatCells[i];
    m_floatCells[i] = nullptr;
  }
  for (int i = 0; i < k_maxNumberOfSequences; i++) {
    delete m_sequenceTitleCells[i];
    m_sequenceTitleCells[i] = nullptr;
  }
  Shared::ValuesController::unloadView(view);
}

}
