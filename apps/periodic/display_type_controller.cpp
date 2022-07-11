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
  size_t displayTypeIndex = 0;
  const DisplayType * currentDisplayType = App::app()->elementsViewDataSource()->displayType();
  while (k_displayTypes[displayTypeIndex] != currentDisplayType) {
    displayTypeIndex++;
    assert(displayTypeIndex < k_numberOfRows);
  }
  selectCellAtLocation(0, displayTypeIndex);
}

bool DisplayTypeController::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    int index = selectedRow();
    App::app()->elementsViewDataSource()->setDisplayType(k_displayTypes[index]);
    stackViewController()->pop();
    return true;
  }
  return SelectableListViewController::handleEvent(e);
}

void DisplayTypeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  assert(cell - static_cast<HighlightCell *>(m_cells) < k_numberOfCells * sizeof(m_cells[0]));
  MessageTableCell * messageCell = static_cast<MessageTableCell *>(cell);
  constexpr I18n::Message k_messages[k_numberOfRows] = {
    I18n::Message::GroupOfElementsTitle,
    I18n::Message::BlocksOfElementsTitle,
    I18n::Message::MetalsTitle,
    I18n::Message::MassTitle,
    I18n::Message::ElectronegativityTitle,
    I18n::Message::MeltingPointTitle,
    I18n::Message::BoilingPointTitle,
    I18n::Message::RadiusTitle,
  };
  messageCell->setMessage(k_messages[index]);
}

}
