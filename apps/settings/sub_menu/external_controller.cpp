#include "external_controller.h"

#include <apps/i18n.h>
#include <apps/settings/main_controller.h>
#include <assert.h>
#include <ion/storage.h>
#include <poincare/preferences.h>
#include "../../apps_container.h"
#include "../../global_preferences.h"

using namespace Poincare;
using namespace Shared;

namespace Settings {

ExternalController::ExternalController(Responder *parentResponder):
  GenericSubController(parentResponder),
  m_contentView(&m_selectableTableView)
{
  m_writeSwitchCell.setMessageFont(KDFont::LargeFont);
  m_enabledSwitchCell.setMessageFont(KDFont::LargeFont);
}

bool ExternalController::handleEvent(Ion::Events::Event event) {
  if ((Ion::Events::OK == event || Ion::Events::EXE == event || Ion::Events::Right == event) && selectedRow() == 0) {
    bool externalWasUnlocked = GlobalPreferences::sharedGlobalPreferences()->externalAppWritePermission();
    GlobalPreferences::sharedGlobalPreferences()->setExternalAppWritePermission(!externalWasUnlocked);
    m_selectableTableView.reloadCellAtLocation(0, selectedRow());
    return true;
  } else if ((Ion::Events::OK == event || Ion::Events::EXE == event || Ion::Events::Right == event) && selectedRow() == 1) {
    bool extappWasShowed = GlobalPreferences::sharedGlobalPreferences()->externalAppShown();
    GlobalPreferences::sharedGlobalPreferences()->setExternalAppShown(!extappWasShowed);
    m_selectableTableView.reloadCellAtLocation(0, selectedRow());
    return true;
  }

  return GenericSubController::handleEvent(event);
}

HighlightCell *ExternalController::reusableCell(int index, int type) {
  if (index == 0) {
      return &m_writeSwitchCell;
  }
  assert(index == 1);
  return &m_enabledSwitchCell;
}

int ExternalController::reusableCellCount(int type) {
  assert(type == 0);
  return 2;
}

void ExternalController::willDisplayCellForIndex(HighlightCell *cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);

  if (index == 0) {
    MessageTableCellWithSwitch *myCell = (MessageTableCellWithSwitch *)cell;
    SwitchView *mySwitch = (SwitchView *)myCell->accessoryView();
    mySwitch->setState(GlobalPreferences::sharedGlobalPreferences()->externalAppWritePermission());
  } else if (index == 1) {
    MessageTableCellWithSwitch *myCell = (MessageTableCellWithSwitch *)cell;
    SwitchView *mySwitch = (SwitchView *)myCell->accessoryView();
    mySwitch->setState(GlobalPreferences::sharedGlobalPreferences()->externalAppShown());
  }
}

void ExternalController::didEnterResponderChain(Responder *previousFirstResponder) {
  m_contentView.reload();
  I18n::Message infoMessages[] = {I18n::Message::ExtAppWriteExplanation1, I18n::Message::ExtAppWriteExplanation2, I18n::Message::ExtAppWriteExplanation3};
  m_contentView.setMessages(infoMessages, k_numberOfExplanationMessages);
}
}
