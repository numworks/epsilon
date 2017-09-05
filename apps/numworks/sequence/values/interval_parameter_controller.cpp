#include "interval_parameter_controller.h"
#include <cmath>

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
  I18n::Message labels[k_totalNumberOfCell] = {I18n::Message::NStart, I18n::Message::NEnd, I18n::Message::Step};
  myCell->setMessage(labels[index]);
  FloatParameterController::willDisplayCellForIndex(cell, index);
}

bool IntervalParameterController::setParameterAtIndex(int parameterIndex, double f) {
  if (f < 0) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  double parameter = std::round(f);
  if (parameterIndex == 2 && parameter == 0.0f) {
    parameter = 1.0f;
  }
  return Shared::IntervalParameterController::setParameterAtIndex(parameterIndex, parameter);
}

}

