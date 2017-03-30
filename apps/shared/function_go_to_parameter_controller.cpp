#include "function_go_to_parameter_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>

namespace Shared {

FunctionGoToParameterController::FunctionGoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol) :
  GoToParameterController(parentResponder, graphRange, cursor, symbol),
  m_function(nullptr)
{
}

const char * FunctionGoToParameterController::title() {
  return I18n::translate(I18n::Message::Goto);
}

float FunctionGoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  return m_cursor->x();
}

bool FunctionGoToParameterController::setParameterAtIndex(int parameterIndex, float f) {
  assert(parameterIndex == 0);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = m_function->evaluateAtAbscissa(f, myApp->localContext());
  if (fabsf(f) > k_maxDisplayableFloat || fabsf(y) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (isnan(y) || isinf(y)) {
    app()->displayWarning(I18n::Message::ValueNotReachedByFunction);
    return false;
  }
  m_cursor->moveTo(f, y);
  return true;
}

void FunctionGoToParameterController::setFunction(Function * function) {
  m_function = function;
}

}
