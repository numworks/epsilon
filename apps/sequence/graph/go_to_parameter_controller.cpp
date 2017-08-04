#include "go_to_parameter_controller.h"
#include "../app.h"
#include <assert.h>
#include <cmath>

namespace Sequence {

bool GoToParameterController::setParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex == 0);
  return Shared::FunctionGoToParameterController::setParameterAtIndex(parameterIndex, std::round(f));
}

}

