#pragma once

#include <apps/shared/interval_parameter_controller.h>

namespace Sequence {

class IntervalParameterController : public Shared::IntervalParameterController {
 public:
  using Shared::IntervalParameterController::IntervalParameterController;
  bool setParameterAtIndex(int parameterIndex, double f) override;
};

}  // namespace Sequence
