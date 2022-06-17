#include "code_options_controller.h"
#include <assert.h>
#include "../../global_preferences.h"

using namespace Shared;

namespace Settings {

CodeOptionsController::CodeOptionsController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_preferencesController(this)
{
  m_chevronCellFontSize.setMessageFont(KDFont::LargeFont);
  m_switchCellAutoCompletion.setMessageFont(KDFont::LargeFont);
  m_switchCellSyntaxHighlighting.setMessageFont(KDFont::LargeFont);
}

bool CodeOptionsController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) {
    switch (selectedRow()){
      case 1:
        GlobalPreferences::sharedGlobalPreferences()->setAutocomplete(!GlobalPreferences::sharedGlobalPreferences()->autocomplete());
        m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
        break;
      case 2:
        GlobalPreferences::sharedGlobalPreferences()->setSyntaxhighlighting(!GlobalPreferences::sharedGlobalPreferences()->syntaxhighlighting());
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
  if (index == 0) {
    return &m_chevronCellFontSize;
  }
  else if (index == 1) {
    return &m_switchCellAutoCompletion;
  }
  return &m_switchCellSyntaxHighlighting;
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
  }
#ifdef HAS_CODE
  else if (thisLabel == I18n::Message::Autocomplete) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(GlobalPreferences::sharedGlobalPreferences()->autocomplete());
  }
  else if (thisLabel == I18n::Message::SyntaxHighlighting) {
    MessageTableCellWithSwitch * mySwitchCell = (MessageTableCellWithSwitch *)cell;
    SwitchView * mySwitch = (SwitchView *)mySwitchCell->accessoryView();
    mySwitch->setState(GlobalPreferences::sharedGlobalPreferences()->syntaxhighlighting());
  }
#endif
}

}
