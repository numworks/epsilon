#include "go_to_parameter_controller.h"
#include "../app.h"
#include <assert.h>

namespace Sequence {

bool GoToParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 0);
  return Shared::FunctionGoToParameterController::setParameterAtIndex(parameterIndex, roundf(f));
}

}

