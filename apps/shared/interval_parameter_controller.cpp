#include "interval_parameter_controller.h"
#include <assert.h>

namespace Shared {

IntervalParameterController::IntervalParameterController(Responder * parentResponder, Interval * interval) :
  FloatParameterController(parentResponder, "Valider"),
  m_interval(interval),
  m_intervalCells{PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr), PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr), PointerTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, nullptr)}
{
}

const char * IntervalParameterController::title() const {
  return "Regler l'intervalle";
}

void IntervalParameterController::viewWillAppear() {
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_previousParameters[i] = parameterAtIndex(i);
  }
  FloatParameterController::viewWillAppear();
}

int IntervalParameterController::numberOfRows() {
  return k_totalNumberOfCell+1;
}

void IntervalParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  PointerTableCellWithEditableText * myCell = (PointerTableCellWithEditableText *)cell;
  const char * labels[k_totalNumberOfCell] = {"x Debut", "x Fin", "x Pas"};
  myCell->setText(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

Interval * IntervalParameterController::interval() {
  return m_interval;
}

float IntervalParameterController::previousParameterAtIndex(int index) {
  assert(index >= 0 && index < k_totalNumberOfCell);
  return m_previousParameters[index];
}

float IntervalParameterController::parameterAtIndex(int index) {
  GetterPointer getters[k_totalNumberOfCell] = {&Interval::start, &Interval::end, &Interval::step};
  return (m_interval->*getters[index])();
}

void IntervalParameterController::setParameterAtIndex(int parameterIndex, float f) {
  SetterPointer setters[k_totalNumberOfCell] = {&Interval::setStart, &Interval::setEnd, &Interval::setStep};
  (m_interval->*setters[parameterIndex])(f);
}

HighlightCell * IntervalParameterController::reusableParameterCell(int index, int type) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  return &m_intervalCells[index];
}

int IntervalParameterController::reusableParameterCellCount(int type) {
  return k_totalNumberOfCell;
}

}
