#include "display_type_controller.h"

#include "app.h"

using namespace Escher;

namespace Elements {

DisplayTypeController::DisplayTypeController(
    StackViewController *stackController)
    : SelectableListViewController(stackController) {}

void DisplayTypeController::viewWillAppear() {
  size_t fieldIndex = 0;
  const DataField *currentField = App::app()->elementsViewDataSource()->field();
  while (k_fields[fieldIndex] != currentField) {
    fieldIndex++;
    assert(fieldIndex < k_numberOfRows);
  }
  selectCell(fieldIndex);
}

bool DisplayTypeController::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    int index = selectedRow();
    App::app()->elementsViewDataSource()->setField(k_fields[index]);
    stackViewController()->pop();
    return true;
  }
  return SelectableListViewController::handleEvent(e);
}

void DisplayTypeController::willDisplayCellForIndex(HighlightCell *cell,
                                                    int index) {
  assert(cell - static_cast<HighlightCell *>(m_cells) <
         static_cast<int>(k_numberOfCells * sizeof(m_cells[0])));
  static_cast<MenuCell<MessageTextView> *>(cell)->label()->setMessage(
      k_fields[index]->fieldLegend());
}

}  // namespace Elements
