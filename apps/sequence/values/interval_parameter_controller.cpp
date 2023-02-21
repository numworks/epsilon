#include "interval_parameter_controller.h"

#include <cmath>

using namespace Shared;

namespace Sequence {

bool IntervalParameterController::setParameterAtIndex(int parameterIndex,
                                                      double f) {
  if (f < 0) {
    Escher::Container::activeApp()->displayWarning(
        I18n::Message::ForbiddenValue);
    return false;
  }
  double parameter = std::round(f);
  if (parameterIndex == 2 && parameter == 0.0f) {
    parameter = 1.0f;
  }
  return Shared::IntervalParameterController::setParameterAtIndex(
      parameterIndex, parameter);
}

}  // namespace Sequence
