#ifndef SEQUENCE_GO_TO_PARAMETER_CONTROLLER_H
#define SEQUENCE_GO_TO_PARAMETER_CONTROLLER_H

#include "../../shared/function_go_to_parameter_controller.h"

namespace Sequence {

class GoToParameterController final : public Shared::FunctionGoToParameterController {
public:
  using Shared::FunctionGoToParameterController::FunctionGoToParameterController;
private:
  bool setParameterAtIndex(int parameterIndex, double f) override;
};

}

#endif

