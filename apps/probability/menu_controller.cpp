#include "menu_controller.h"
#include <escher/highlight_cell.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell.h>
#include <assert.h>
#include <apps/i18n.h>
#include <escher/container.h>

namespace Probability {

MenuController::MenuController(Responder * parentResponder) :
 SelectableListViewController(parentResponder),
 m_labels{I18n::Message::ProbaApp, I18n::Message::SignificanceTest, I18n::Message::ConfidenceInterval}
{ }

void MenuController::willDisplayCellForIndex(Escher::HighlightCell * cell, int row) {
  Escher::MessageTableCell * msgCell = static_cast<Escher::MessageTableCell *>(cell);
  msgCell->setMessage(m_labels[row]);
}

void MenuController::didBecomeFirstResponder() {
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

Escher::HighlightCell * MenuController::reusableCell(int index, int type) {
  // Return correct cell
  assert(type == 0);
  assert(index >= 0 && index <= k_numberOfCells);
  return &m_cells[index];
}



}