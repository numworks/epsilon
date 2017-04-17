#include "interval_parameter_controller.h"
#include <assert.h>

namespace Shared {

IntervalParameterController::IntervalParameterController(Responder * parentResponder, Interval * interval) :
  FloatParameterController(parentResponder),
  m_interval(interval),
  m_intervalCells{MessageTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, I18n::Message::Default), MessageTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, I18n::Message::Default), MessageTableCellWithEditableText(&m_selectableTableView, this, m_draftTextBuffer, I18n::Message::Default)}
{
}

const char * IntervalParameterController::title() {
  return I18n::translate(I18n::Message::IntervalSet);
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
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::XStart, I18n::Message::XEnd, I18n::Message::Step};
  myCell->setMessage(labels[index]);
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

bool IntervalParameterController::setParameterAtIndex(int parameterIndex, float f) {
  if (f <= 0.0f && parameterIndex == 2) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  float start = parameterIndex == 0 ? f : m_interval->start();
  float end = parameterIndex == 1 ? f : m_interval->end();
  if (start > end) {
    if (parameterIndex == 1) {
      app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    float g = f+1.0f;
    m_interval->setEnd(g);
  }
  SetterPointer setters[k_totalNumberOfCell] = {&Interval::setStart, &Interval::setEnd, &Interval::setStep};
  (m_interval->*setters[parameterIndex])(f);
  return true;
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
