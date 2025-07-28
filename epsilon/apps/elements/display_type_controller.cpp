#include "display_type_controller.h"

#include "app.h"

using namespace Escher;

namespace Elements {

DisplayTypeController::DisplayTypeController(
    StackViewController* stackController)
    : UniformSelectableListController(stackController) {
  for (int i = 0; i < k_numberOfCells; ++i) {
    cell(i)->label()->setMessage(k_fields[i]->fieldLegend());
  }
}

void DisplayTypeController::viewWillAppear() {
  size_t fieldIndex = 0;
  const DataField* currentField = App::app()->elementsViewDataSource()->field();
  while (k_fields[fieldIndex] != currentField) {
    fieldIndex++;
    assert(fieldIndex < k_numberOfCells);
  }
  selectRow(fieldIndex);
}

bool DisplayTypeController::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    int index = selectedRow();
    App::app()->elementsViewDataSource()->setField(k_fields[index]);
    stackViewController()->pop();
    return true;
  }
  return false;
}

}  // namespace Elements
