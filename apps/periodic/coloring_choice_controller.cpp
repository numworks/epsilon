#include "coloring_choice_controller.h"
#include "app.h"

using namespace Escher;

namespace Periodic {

ColoringChoiceController::ColoringChoiceController(StackViewController * stackController) :
  SelectableListViewController(stackController)
{
  for (MessageTableCell & cell : m_cells) {
    cell.setTextColor(KDColorBlack);
    cell.setBackgroundColor(KDColorWhite);
    cell.setMessageFont(KDFont::Size::Large);
  }
}

void ColoringChoiceController::viewWillAppear() {
  size_t coloringIndex = 0;
  const Coloring * currentColoring = App::app()->elementsViewDataSource()->coloring();
  while (k_colorings[coloringIndex] != currentColoring) {
    coloringIndex++;
    assert(coloringIndex < k_numberOfRows);
  }
  selectCellAtLocation(0, coloringIndex);
}

bool ColoringChoiceController::handleEvent(Ion::Events::Event e) {
  if (e == Ion::Events::OK || e == Ion::Events::EXE) {
    int index = selectedRow();
    App::app()->elementsViewDataSource()->setColoring(k_colorings[index]);
    stackViewController()->pop();
    return true;
  }
  return SelectableListViewController::handleEvent(e);
}

void ColoringChoiceController::willDisplayCellForIndex(HighlightCell * cell, int index) {
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
