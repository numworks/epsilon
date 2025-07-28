#include "interval_parameter_controller.h"

using namespace Escher;

namespace Shared {

Interval::IntervalParameters*
IntervalParameterController::SharedTempIntervalParameters() {
  static Interval::IntervalParameters sTempIntervalParameters;
  return &sTempIntervalParameters;
}

IntervalParameterController::IntervalParameterController(
    Responder* parentResponder)
    : FloatParameterController<double>(parentResponder),
      m_interval(nullptr),
      m_title(I18n::Message::IntervalSet),
      m_confirmPopUpController(Invocation::Builder<IntervalParameterController>(
          [](IntervalParameterController* controller, void* sender) {
            controller->stackController()->pop();
            return true;
          },
          this)) {
  I18n::Message messages[k_totalNumberOfCell] = {
      I18n::Message::XStart, I18n::Message::XEnd, I18n::Message::Step};
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_intervalCells[i].setParentResponder(&m_selectableListView);
    m_intervalCells[i].setDelegate(this);
    m_intervalCells[i].label()->setMessage(messages[i]);
  }
}

void IntervalParameterController::setInterval(Interval* interval) {
  m_interval = interval;
  *SharedTempIntervalParameters() = *(interval->parameters());
}

const char* IntervalParameterController::title() const {
  return I18n::translate(m_title);
}

int IntervalParameterController::numberOfRows() const {
  return k_totalNumberOfCell + 1;
}

KDCoordinate IntervalParameterController::nonMemoizedRowHeight(int row) {
  int type = typeAtRow(row);
  if (type == k_parameterCellType) {
    return m_intervalCells[row].minimalSizeForOptimalDisplay().height();
  }
  assert(type == k_buttonCellType);
  return Shared::FloatParameterController<double>::nonMemoizedRowHeight(row);
}

void IntervalParameterController::setStartEndMessages(
    I18n::Message startMessage, I18n::Message endMessage) {
  m_intervalCells[0].label()->setMessage(startMessage);
  m_intervalCells[1].label()->setMessage(endMessage);
}

double IntervalParameterController::parameterAtIndex(int index) {
  GetterPointer getters[k_totalNumberOfCell] = {
      &Interval::IntervalParameters::start, &Interval::IntervalParameters::end,
      &Interval::IntervalParameters::step};
  return (SharedTempIntervalParameters()->*getters[index])();
}

bool IntervalParameterController::setParameterAtIndex(int parameterIndex,
                                                      double f) {
  if (f <= 0.0f && parameterIndex == 2) {
    App::app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  double start =
      parameterIndex == 0 ? f : SharedTempIntervalParameters()->start();
  double end = parameterIndex == 1 ? f : SharedTempIntervalParameters()->end();
  if (start > end) {
    if (parameterIndex == 1) {
      App::app()->displayWarning(I18n::Message::ForbiddenValue);
      return false;
    }
    double g = f + 1.0;
    SharedTempIntervalParameters()->setEnd(g);
  }
  SetterPointer setters[k_totalNumberOfCell] = {
      &Interval::IntervalParameters::setStart,
      &Interval::IntervalParameters::setEnd,
      &Interval::IntervalParameters::setStep};
  (SharedTempIntervalParameters()->*setters[parameterIndex])(f);
  return true;
}

HighlightCell* IntervalParameterController::reusableParameterCell(int index,
                                                                  int type) {
  assert(index >= 0);
  assert(index < k_totalNumberOfCell);
  return &m_intervalCells[index];
}

TextField* IntervalParameterController::textFieldOfCellAtIndex(
    HighlightCell* cell, int index) {
  assert(typeAtRow(index) == k_parameterCellType);
  return static_cast<MenuCellWithEditableText<MessageTextView>*>(cell)
      ->textField();
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

int IntervalParameterController::reusableParameterCellCount(int type) const {
  return k_totalNumberOfCell;
}

void IntervalParameterController::buttonAction() {
  m_interval->setParameters(*SharedTempIntervalParameters());
  m_interval->forceRecompute();
  StackViewController* stack = stackController();
  stack->popUntilDepth(1, true);
}

}  // namespace Shared
