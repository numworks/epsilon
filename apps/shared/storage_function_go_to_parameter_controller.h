#ifndef SHARED_STORAGE_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_STORAGE_FUNCTION_GO_TO_PARAMETER_CONTROLLER_H

#include "go_to_parameter_controller.h"

namespace Shared {

template<class T>
class StorageFunctionGoToParameterController : public GoToParameterController {
public:
  StorageFunctionGoToParameterController(Responder * parentResponder, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol)  :
    GoToParameterController(parentResponder, graphRange, cursor, symbol),
    m_function()
  {}

  const char * title() override {
    return I18n::translate(I18n::Message::Goto);
  }
  void setFunction(T * function) {
    m_function = *function;
  }
protected:
  bool setParameterAtIndex(int parameterIndex, double f) override {
    assert(parameterIndex == 0);
    TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
    float y = m_function.evaluateAtAbscissa(f, myApp->localContext());
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
  T m_function;
private:
  double parameterAtIndex(int index) override {
    assert(index == 0);
    return m_cursor->x();
  }
};

}

#endif
