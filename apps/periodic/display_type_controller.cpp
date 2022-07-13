#include "display_type_controller.h"
#include "app.h"

using namespace Escher;

namespace Periodic {

DisplayTypeController::DisplayTypeController(StackViewController * stackController) :
  SelectableListViewController(stackController)
{
  for (MessageTableCell & cell : m_cells) {
    cell.setTextColor(KDColorBlack);
    cell.setBackgroundColor(KDColorWhite);
    cell.setMessageFont(KDFont::Size::Large);
  }
}

void DisplayTypeController::viewWillAppear() {
  size_t fieldIndex = 0;
  const DataField * currentField = App::app()->elementsViewDataSource()->field();
  while (k_fields[fieldIndex] != currentField) {
    fieldIndex++;
    assert(fieldIndex < k_numberOfRows);
  }
  selectCellAtLocation(0, fieldIndex);
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

void DisplayTypeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(cell - static_cast<HighlightCell *>(m_cells) < k_numberOfCells * sizeof(m_cells[0]));
  MessageTableCell * messageCell = static_cast<MessageTableCell *>(cell);
  messageCell->setMessage(k_fields[index]->fieldLegend());
}

}
