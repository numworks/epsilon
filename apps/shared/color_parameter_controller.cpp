#include "color_parameter_controller.h"
#include <apps/i18n.h>
#include "function_app.h"

using namespace Escher;

namespace Shared {

void ColorParameterController::viewWillAppear() {
  int functionColorIndex = 0;
  KDColor functionColor = function()->color();
  for (int i=0; i<ColorNames::Count; i++) {
    if (functionColor == ColorNames::Colors[i]) {
      functionColorIndex = i;
      break;
    }
  }
  selectRow(functionColorIndex);
}

bool ColorParameterController::handleEvent(Ion::Events::Event event) {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    KDColor selectedColor = ColorNames::Colors[selectedRow()];
    function()->setColor(selectedColor);
    stack->pop();
    stack->pop(); // Pop all the way back
    return true;
  } else if (event == Ion::Events::Left) {
    stack->pop();
    return true;
  }
  return false;
}

void ColorParameterController::didBecomeFirstResponder() {
  // TODO: This is duplicated in other classes
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void ColorParameterController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  ColorCell * colorCell = static_cast<ColorCell *>(cell);
  assert(index >= 0);
  assert(index < ColorNames::Count);
  colorCell->setMessage(ColorNames::Messages[index]);
  colorCell->setColor(ColorNames::Colors[index]);
}

ExpiringPointer<Function> ColorParameterController::function() {
  return FunctionApp::app()->functionStore()->modelForRecord(m_record);
}

}
