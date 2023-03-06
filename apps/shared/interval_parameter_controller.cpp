#include "interval_parameter_controller.h"

using namespace Escher;

namespace Shared {

Interval::IntervalParameters *
IntervalParameterController::SharedTempIntervalParameters() {
  static Interval::IntervalParameters sTempIntervalParameters;
  return &sTempIntervalParameters;
}

IntervalParameterController::IntervalParameterController(
    Responder *parentResponder,
    InputEventHandlerDelegate *inputEventHandlerDelegate)
    : FloatParameterController<double>(parentResponder),
      m_interval(nullptr),
      m_title(I18n::Message::IntervalSet),
      m_startMessage(I18n::Message::XStart),
      m_endMessage(I18n::Message::XEnd),
      m_confirmPopUpController(Invocation::Builder<IntervalParameterController>(
          [](IntervalParameterController *controller, void *sender) {
            controller->stackController()->pop();
            return true;
          },
          this)) {
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_intervalCells[i].setParentResponder(&m_selectableListView);
    m_intervalCells[i].setDelegates(inputEventHandlerDelegate, this);
  }
}

void IntervalParameterController::setInterval(Interval *interval) {
  m_interval = interval;
  *SharedTempIntervalParameters() = *(interval->parameters());
}

const char *IntervalParameterController::title() {
  return I18n::translate(m_title);
}

int IntervalParameterController::numberOfRows() const {
  return k_totalNumberOfCell + 1;
}

void IntervalParameterController::setStartEndMessages(
    I18n::Message startMessage, I18n::Message endMessage) {
  m_startMessage = startMessage;
  m_endMessage = endMessage;
}

void IntervalParameterController::willDisplayCellForIndex(HighlightCell *cell,
                                                          int index) {
  if (index == numberOfRows() - 1) {
    return;
  }

  MessageTableCellWithEditableText *myCell =
      static_cast<MessageTableCellWithEditableText *>(cell);
  assert(index >= 0 && index < 3);
  I18n::Message m = index == 0
                        ? m_startMessage
                        : (index == 1 ? m_endMessage : I18n::Message::Step);
  myCell->setMessage(m);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

double IntervalParameterController::parameterAtIndex(int index) {
  GetterPointer getters[k_totalNumberOfCell] = {
      &Shared::Interval::IntervalParameters::start,
      &Shared::Interval::IntervalParameters::end,
      &Shared::Interval::IntervalParameters::step};
  return (SharedTempIntervalParameters()->*getters[index])();
}

bool IntervalParameterController::setParameterAtIndex(int parameterIndex,
                                                      double f) {
  if (f <= 0.0f && parameterIndex == 2) {
    Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  double start =
      parameterIndex == 0 ? f : SharedTempIntervalParameters()->start();
  double end = parameterIndex == 1 ? f : SharedTempIntervalParameters()->end();
  if (start > end) {
    if (parameterIndex == 1) {
      Container::activeApp()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    double g = f + 1.0;
    SharedTempIntervalParameters()->setEnd(g);
  }
  SetterPointer setters[k_totalNumberOfCell] = {
      &Shared::Interval::IntervalParameters::setStart,
      &Shared::Interval::IntervalParameters::setEnd,
      &Shared::Interval::IntervalParameters::setStep};
  (SharedTempIntervalParameters()->*setters[parameterIndex])(f);
  return true;
}

HighlightCell *IntervalParameterController::reusableParameterCell(int index,
                                                                  int type) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  return &m_intervalCells[index];
}

bool IntervalParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Left && stackController()->depth() > 2) {
    stackController()->pop();
    return true;
  }
  if (event == Ion::Events::Back &&
      !m_interval->hasSameParameters(*SharedTempIntervalParameters())) {
    // Open pop-up to confirm discarding values
    m_confirmPopUpController.presentModally();
    return true;
  }
  return false;
}

int IntervalParameterController::reusableParameterCellCount(int type) {
  return k_totalNumberOfCell;
}

void IntervalParameterController::buttonAction() {
  m_interval->setParameters(*SharedTempIntervalParameters());
  m_interval->forceRecompute();
  StackViewController *stack = stackController();
  stack->popUntilDepth(1, true);
}

}  // namespace Shared
