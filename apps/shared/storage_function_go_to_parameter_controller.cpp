#include "storage_function_go_to_parameter_controller.h"
#include "storage_function_app.h"
#include <assert.h>
#include <cmath>

namespace Shared {

StorageFunctionGoToParameterController::StorageFunctionGoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor, I18n::Message symbol) :
  GoToParameterController(parentResponder, inputEventHandlerDelegate, graphRange, cursor, symbol),
  m_record()
{
}

const char * StorageFunctionGoToParameterController::title() {
  return I18n::translate(I18n::Message::Goto);
}

double StorageFunctionGoToParameterController::parameterAtIndex(int index) {
  assert(index == 0);
  return m_cursor->x();
}

bool StorageFunctionGoToParameterController::setParameterAtIndex(int parameterIndex, double f) {
  assert(parameterIndex == 0);
  StorageFunctionApp * myApp = (StorageFunctionApp *)app();
  ExpiringPointer<StorageFunction> function = myApp->functionStore()->modelForRecord(m_record);
  float y = function->evaluateAtAbscissa(f, myApp->localContext());
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

void StorageFunctionGoToParameterController::setRecord(Ion::Storage::Record record) {
  m_record = record;
}

}
