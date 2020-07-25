#include "interval_parameter_controller.h"

namespace Shared {

static Interval::intervalParameters s_tempIntevalParameters;

IntervalParameterController::IntervalParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate) :
  FloatParameterController<double>(parentResponder),
  m_interval(nullptr),
  m_intervalCells{},
  m_title(I18n::Message::IntervalSet),
  m_startMessage(I18n::Message::XStart),
  m_endMessage(I18n::Message::XEnd)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_intervalCells[i].setParentResponder(&m_selectableTableView);
    m_intervalCells[i].textField()->setDelegates(inputEventHandlerDelegate, this);
  }
}

void IntervalParameterController::setInterval(Interval * interval) { 
  m_interval = interval;
  s_tempIntevalParameters = *(*interval).parameters();
}

const char * IntervalParameterController::title() {
  return I18n::translate(m_title);
}

int IntervalParameterController::numberOfRows() const {
  return k_totalNumberOfCell+1;
}

void IntervalParameterController::setStartEndMessages(I18n::Message startMessage, I18n::Message endMessage) {
  m_startMessage = startMessage;
  m_endMessage = endMessage;
}

void IntervalParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }

  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  assert(index >= 0 && index < 3);
  I18n::Message m = index == 0 ? m_startMessage : (index == 1 ? m_endMessage : I18n::Message::Step);
  myCell->setMessage(m);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

double IntervalParameterController::parameterAtIndex(int index) {
  GetterPointer getters[k_totalNumberOfCell] = {&Shared::Interval::intervalParameters::start, &Shared::Interval::intervalParameters::end, &Shared::Interval::intervalParameters::step};
  return (s_tempIntevalParameters.*getters[index])();
}

bool IntervalParameterController::setParameterAtIndex(int parameterIndex, double f) {
  if (f <= 0.0f && parameterIndex == 2) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  double start = parameterIndex == 0 ? f : s_tempIntevalParameters.start();
  double end = parameterIndex == 1 ? f : s_tempIntevalParameters.end();
  if (start > end) {
    if (parameterIndex == 1) {
      Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    double g = f+1.0;
    s_tempIntevalParameters.setEnd(g);
  }
  SetterPointer setters[k_totalNumberOfCell] = {&Shared::Interval::intervalParameters::setStart, &Shared::Interval::intervalParameters::setEnd, &Shared::Interval::intervalParameters::setStep};
  (s_tempIntevalParameters.*setters[parameterIndex])(f);
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
  m_interval->setParameters(s_tempIntevalParameters);
  m_interval->forceRecompute();
  StackViewController * stack = stackController();
  stack->pop();
  if (stack->depth() > 1) {
    stack->pop();
  }
}

}
