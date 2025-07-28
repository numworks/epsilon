#include "go_to_parameter_controller.h"

#include <assert.h>

#include <cmath>

namespace Sequence {

void GoToParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
  setParameterName(I18n::Message::N);
}

bool GoToParameterController::setParameterAtIndex(int parameterIndex,
                                                  double f) {
  assert(parameterIndex == 0);
  return Shared::FunctionGoToParameterController::setParameterAtIndex(
      parameterIndex, std::fmax(0, std::round(f)));
}

}  // namespace Sequence
