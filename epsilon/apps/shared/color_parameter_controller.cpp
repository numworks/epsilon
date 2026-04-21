#include "color_parameter_controller.h"

#include "function_app.h"

using namespace Escher;

namespace Shared {

int ColorParameterController::currentColorIndex() const {
  ThemeComponent functionColor = function()->color(m_derivationOrder);
  for (int i = 0; i < ColorNames::k_count; i++) {
    if (functionColor == GetColor(i)) {
      return i;
    }
  }
  return 0;
}

void ColorParameterController::setColorIndex(int colorIndex) {
  function()->setColor(GetColor(colorIndex), m_derivationOrder);
}

void ColorParameterController::popAfterColorSelection(
    StackViewController* stack) {
  stack->popUntilDepth(
      Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
      true);
}

void ColorParameterController::setRecord(Ion::Storage::Record record,
                                         int derivationOrder) {
  m_record = record;
  m_derivationOrder = derivationOrder;
}

OMG::ExpiringPointer<Function> ColorParameterController::function() const {
  return FunctionApp::app()->functionStore().modelForRecord(m_record);
}

}  // namespace Shared
