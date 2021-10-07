#include "code_options_controller.h"
#include <assert.h>
#include "../../global_preferences.h"

using namespace Shared;

namespace Settings {

CodeOptionsController::CodeOptionsController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_preferencesController(this)
{
  for (int i = 0; i < k_totalNumberOfCell; i++) {
    m_cells[i].setMessageFont(KDFont::LargeFont);
  }
  for (int i = 0; i < k_totalNumberOfSwitchCells; i++) {
    m_switchCells[i].setMessageFont(KDFont::LargeFont);
  }
}

bool CodeOptionsController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    switch (selectedRow()){
      case 1:
        GlobalPreferences::sharedGlobalPreferences()->setAutocomplete(!GlobalPreferences::sharedGlobalPreferences()->autocomplete());
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        break;

      default:
        GenericSubController * subController = nullptr;
        subController = &m_preferencesController;
        subController->setMessageTreeModel(m_messageTreeModel->childAtIndex(selectedRow()));
        StackViewController * stack = stackController();
        m_lastSelect = selectedRow();
        stack->push(subController);
        break;
        
    }
    return true;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell * CodeOptionsController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0 && index < k_totalNumberOfCell);
  return &m_cells[index];
}

int CodeOptionsController::reusableCellCount(int type) {
  assert(type == 0);
  return k_totalNumberOfCell;
}

void CodeOptionsController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);
  I18n::Message thisLabel = m_messageTreeModel->childAtIndex(index)->label();

  if (thisLabel == I18n::Message::FontSizes){
    MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
    myTextCell->setMessage(thisLabel);
    GlobalPreferences::sharedGlobalPreferences()->font() == KDFont::LargeFont
        ? myTextCell->setSubtitle(I18n::Message::LargeFont) 
        : myTextCell->setSubtitle(I18n::Message::SmallFont);
  } else if (thisLabel == I18n::Message::Autocomplete) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(GlobalPreferences::sharedGlobalPreferences()->autocomplete());
  }
}

}
