#ifndef SHARED_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H

#include "go_to_parameter_controller.h"
#include "function.h"

namespace Shared {

class FunctionGoToParameterController : public GoToParameterController {
public:
  FunctionGoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol);
  const char * title() override;
  void setFunction(Function * function);
protected:
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Function * m_function;
private:
  double parameterAtIndex(int index) override;
};

}

#endif
