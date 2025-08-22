#pragma once

#include <apps/shared/function_go_to_parameter_controller.h>

namespace Sequence {

class GoToParameterController : public Shared::FunctionGoToParameterController {
 public:
  using Shared::FunctionGoToParameterController::
      FunctionGoToParameterController;

  void setRecord(Ion::Storage::Record record);

 private:
  bool setParameterAtIndex(int parameterIndex, double f) override;
};

}  // namespace Sequence
