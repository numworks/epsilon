#include "usb_info_controller.h"

#include <apps/i18n.h>
#include <apps/settings/main_controller.h>
#include <assert.h>
#include <ion/storage.h>
#include <poincare/preferences.h>

#include <cmath>

#include "../../apps_container.h"
#include "../../global_preferences.h"

using namespace Poincare;
using namespace Shared;

namespace Settings {

UsbInfoController::UsbInfoController(Responder *parentResponder): 
  GenericSubController(parentResponder),
  m_usbProtectionLevelController(this),
  m_dfuLevel(KDFont::LargeFont, KDFont::SmallFont)
{
  for (int i = 0; i < k_maxSwitchCells; i++) {
    m_switchCells[i].setMessageFont(KDFont::LargeFont);
  }
}

bool UsbInfoController::handleEvent(Ion::Events::Event event) {
  if ((Ion::Events::OK == event || Ion::Events::EXE == event) && selectedRow() == 0) {
    if (!GlobalPreferences::sharedGlobalPreferences()->dfuStatus()) {
      if (!GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
        Ion::LED::setColor(KDColorPurple);
        Ion::LED::setBlinking(500, 0.5f);
      }
      GlobalPreferences::sharedGlobalPreferences()->setDfuStatus(true);
      Container::activeApp()->displayWarning(I18n::Message::DfuWarning1, I18n::Message::DfuWarning2);
    } else {
      if (!GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
        Ion::LED::setColor(KDColorBlack);
      }
      GlobalPreferences::sharedGlobalPreferences()->setDfuStatus(false);
    }
    m_selectableTableView.reloadCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
    AppsContainer::sharedAppsContainer()->redrawWindow(true);
    return true;
  }
  // We cannot use things like willExitResponderChain because this view can disappear due to an USB connection,
  // and in this case we must keep the DFU status.
  if ((Ion::Events::Left == event || Ion::Events::Home == event) && GlobalPreferences::sharedGlobalPreferences()->dfuStatus()) {
    Container::activeApp()->displayWarning(I18n::Message::USBProtectionReanabled);
    GlobalPreferences::sharedGlobalPreferences()->setDfuStatus(false);
  }
  if ((Ion::Events::OK == event || Ion::Events::EXE == event) && selectedRow() == 1) {
    GenericSubController *subController = &m_usbProtectionLevelController;
    subController->setMessageTreeModel(m_messageTreeModel->childAtIndex(1));
    StackViewController *stack = stackController();
    m_lastSelect = selectedRow();
    stack->push(subController);
    return true;
  }
  return GenericSubController::handleEvent(event);
}

HighlightCell *UsbInfoController::reusableCell(int index, int type) {
  assert(index == 0 || index == 1);
  return index == 0 ? &m_switchCell : &m_dfuLevelCell;
}

int UsbInfoController::reusableCellCount(int type) {
  assert(type == 0);
  return 1;
}

void UsbInfoController::willDisplayCellForIndex(HighlightCell *cell, int index) {
  GenericSubController::willDisplayCellForIndex(cell, index);

  if (index == 0) {
    MessageTableCellWithSwitch *myCell = (MessageTableCellWithSwitch *)cell;
    SwitchView *mySwitch = (SwitchView *)myCell->accessoryView();
    mySwitch->setState(!GlobalPreferences::sharedGlobalPreferences()->dfuStatus());
  } else if (index == 1) {
    MessageTableCellWithChevronAndMessage *mcell = (MessageTableCellWithChevronAndMessage *)cell;
    int currentLevel = GlobalPreferences::sharedGlobalPreferences()->getDfuLevel();
    if (currentLevel == 0) {
      mcell->setSubtitle(I18n::Message::USBDefaultLevelDesc);
    } else if (currentLevel == 1) {;
      mcell->setSubtitle(I18n::Message::USBLowLevelDesc);
    } else {
      assert(currentLevel == 2);
      mcell->setSubtitle(I18n::Message::USBParanoidLevelDesc);
    }
  }
}

void UsbInfoController::didEnterResponderChain(Responder *previousFirstResponder) {
  m_selectableTableView.reload();
  I18n::Message infoMessages[] = {I18n::Message::USBExplanation1, I18n::Message::USBExplanation2, I18n::Message::USBExplanation3};
  m_selectableTableView.setMessages(infoMessages, k_numberOfExplanationMessages);
}
}  
