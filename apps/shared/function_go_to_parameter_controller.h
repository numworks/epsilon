#ifndef SHARED_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H

#include "go_to_parameter_controller.h"
#include "function.h"

namespace Shared {

class FunctionGoToParameterController : public GoToParameterController {
public:
  FunctionGoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol) :
    GoToParameterController(parentResponder, graphRange, cursor, symbol),
    m_function(nullptr) {}
  const char * title() override {
    return I18n::translate(I18n::Message::Goto);
  }
  void setFunction(Function * function) {
    m_function = function;
  }
protected:
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Function * m_function;
private:
  double parameterAtIndex(int index) override;
};

}

#endif
