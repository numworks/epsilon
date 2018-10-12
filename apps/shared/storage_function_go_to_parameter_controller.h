#ifndef SHARED_STORAGE_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_STORAGE_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H

#include "go_to_parameter_controller.h"
#include "storage_function.h"

namespace Shared {

class StorageFunctionGoToParameterController : public GoToParameterController {
public:
  StorageFunctionGoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol);
  const char * title() override;
  void setFunction(StorageFunction * function);
protected:
  bool setParameterAtIndex(int parameterIndex, double f) override;
  StorageFunction * m_function;
private:
  double parameterAtIndex(int index) override;
};

}

#endif
