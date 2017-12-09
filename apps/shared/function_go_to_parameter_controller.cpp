#include "function_go_to_parameter_controller.h"
#include "text_field_delegate_app.h"
#include <assert.h>
#include <cmath>

namespace Shared {

double FunctionGoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  return m_cursor->x();
}

bool FunctionGoToParameterController::setParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex == 0);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = m_function->evaluateAtAbscissa(f, myApp->localContext());
  if (std::fabs(f) > k_maxDisplayableFloat || std::fabs(y) > k_maxDisplayableFloat) {
    app()->displayWarning(I18n::Message::ForbiddenValue);
    return false;
  }
  if (std::isnan(y) || std::isinf(y)) {
    app()->displayWarning(I18n::Message::ValueNotReachedByFunction);
    return false;
  }
  m_cursor->moveTo(f, y);
  m_graphRange->centerAxisAround(CurveViewRange::Axis::X, m_cursor->x());
  m_graphRange->centerAxisAround(CurveViewRange::Axis::Y, m_cursor->y());
  return true;
}

}
