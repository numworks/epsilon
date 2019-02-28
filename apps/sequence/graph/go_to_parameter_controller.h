#ifndef SEQUENCE_GO_TO_PARAMETER_CONTROLLER_H
#define SEQUENCE_GO_TO_PARAMETER_CONTROLLER_H

#include "../../shared/storage_function_go_to_parameter_controller.h"

namespace Sequence {

class GoToParameterController : public Shared::StorageFunctionGoToParameterController {
public:
  using Shared::StorageFunctionGoToParameterController::StorageFunctionGoToParameterController;
private:
  bool setParameterAtIndex(int parameterIndex, double f) override;
};

}

#endif

