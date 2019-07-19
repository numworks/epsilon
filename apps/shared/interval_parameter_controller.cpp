#include "interval_parameter_controller.h"
#include <assert.h>

namespace Shared {

IntervalParameterController::IntervalParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Interval * interval) :
  FloatParameterController(parentResponder),
  m_interval(interval),
  m_intervalCells{}
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_intervalCells[i].setParentResponder(&m_selectableTableView);
    m_intervalCells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
    m_intervalCells[i].textField()->setDraftTextBuffer(m_draftTextBuffer);
  }
}

const char * IntervalParameterController::title() {
  return I18n::translate(I18n::Message::IntervalSet);
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

double IntervalParameterController::parameterAtIndex(int index) {
  GetterPointer getters[k_totalNumberOfCell] = {&Interval::start, &Interval::end, &Interval::step};
  return (m_interval->*getters[index])();
}

bool IntervalParameterController::setParameterAtIndex(int parameterIndex, double f) {
  if (f <= 0.0f && parameterIndex == 2) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  double start = parameterIndex == 0 ? f : m_interval->start();
  double end = parameterIndex == 1 ? f : m_interval->end();
  if (start > end) {
    if (parameterIndex == 1) {
      Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    double g = f+1.0;
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

bool IntervalParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left && stackController()->depth() > 2) {
    stackController()->pop();
    return true;
  }
  return false;
}

int IntervalParameterController::reusableParameterCellCount(int type) {
  return k_totalNumberOfCell;
}

void IntervalParameterController::buttonAction() {
  StackViewController * stack = stackController();
  stack->pop();
  if (stack->depth() > 1) {
    stack->pop();
  }
}

}
