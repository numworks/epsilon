#include "interval_parameter_controller.h"

using namespace Shared;

namespace Sequence {

IntervalParameterController::IntervalParameterController(Responder * parentResponder, Interval * interval) :
  Shared::IntervalParameterController(parentResponder, interval)
{
}

void IntervalParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == numberOfRows()-1) {
    return;
  }
  MessageTableCellWithEditableText * myCell = (MessageTableCellWithEditableText *)cell;
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::XStart, I18n::Message::XEnd, I18n::Message::XStep};
  myCell->setMessage(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool IntervalParameterController::setParameterAtIndex(int parameterIndex, float f) {
  if (f <= 0) {
    return false;
  }
  SetterPointer setters[k_totalNumberOfCell] = {&Interval::setStart, &Interval::setEnd, &Interval::setStep};
  float parameter = roundf(f);
  if (parameterIndex == 2 && parameter == 0.0f) {
    parameter = 1.0f;
  }
  (m_interval->*setters[parameterIndex])(parameter);
  return true;
}

}

