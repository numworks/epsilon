#include "color_parameter_controller.h"

#include <apps/i18n.h>

#include "function_app.h"

using namespace Escher;

namespace Shared {

void ColorParameterController::viewWillAppear() {
  int functionColorIndex = 0;
  KDColor functionColor = function()->color();
  for (int i = 0; i < ColorNames::k_count; i++) {
    if (functionColor == ColorNames::k_colors[i]) {
      functionColorIndex = i;
      break;
    }
  }
  selectRow(functionColorIndex);
}

bool ColorParameterController::handleEvent(Ion::Events::Event event) {
  StackViewController *stack =
      static_cast<StackViewController *>(parentResponder());
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    KDColor selectedColor = ColorNames::k_colors[selectedRow()];
    function()->setColor(selectedColor);
    // Pop all the way back
    stack->popUntilDepth(
        Shared::InteractiveCurveViewController::k_graphControllerStackDepth,
        true);
    return true;
  } else if (event == Ion::Events::Left) {
    stack->pop();
    return true;
  }
  return false;
}

void ColorParameterController::willDisplayCellForIndex(HighlightCell *cell,
                                                       int index) {
  ColorCell *colorCell = static_cast<ColorCell *>(cell);
  assert(index >= 0);
  assert(index < ColorNames::k_count);
  colorCell->setMessage(ColorNames::k_messages[index]);
  colorCell->setColor(ColorNames::k_colors[index]);
}

KDCoordinate ColorParameterController::defaultRowHeight() {
  // Use the first row as template for all heights.
  ColorCell tempCell;
  return heightForCellAtIndexWithWidthInit(&tempCell, 0);
}

ExpiringPointer<Function> ColorParameterController::function() {
  return FunctionApp::app()->functionStore()->modelForRecord(m_record);
}

}  // namespace Shared
