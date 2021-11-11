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

UsbInfoController::UsbInfoController(Responder *parentResponder) : GenericSubController(parentResponder),
                                                                   m_usbprotectlevel(this),
                                                                   m_dfuLevel(KDFont::LargeFont, KDFont::SmallFont),
                                                                   m_contentView(&m_selectableTableView) {
    for (int i = 0; i < k_maxSwitchCells; i++) {
        m_switchCells[i].setMessageFont(KDFont::LargeFont);
        //Ancien code au cas ou on souhaite ajouter d'autres éléments dans le menu
        // m_cell[i].setMessageFont(KDFont::LargeFont);
        // m_cell[i].setAccessoryFont(KDFont::SmallFont);
        // m_cell[i].setAccessoryTextColor(Palette::SecondaryText);
    }
}

bool UsbInfoController::handleEvent(Ion::Events::Event event) {
    if ((Ion::Events::OK == event || Ion::Events::EXE == event) && selectedRow() == 0) {
        if(GlobalPreferences::sharedGlobalPreferences()->showDfuDeacAlert()){
            GlobalPreferences::sharedGlobalPreferences()->setDfuDeacAlert(false);
            Container::activeApp()->displayWarning(I18n::Message::USBDeacAlert1, I18n::Message::USBDeacAlert2);
            return true;
        }
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

    if (GlobalPreferences::sharedGlobalPreferences()->dfuStatus() && event != Ion::Events::USBPlug && event != Ion::Events::USBEnumeration) {
        Container::activeApp()->displayWarning(I18n::Message::UsbSetting, I18n::Message::USBSettingDeact);
        return true;
    }

    if ((Ion::Events::OK == event || Ion::Events::EXE == event) && selectedRow() == 1) {
        GenericSubController *subController = &m_usbprotectlevel;
        subController->setMessageTreeModel(m_messageTreeModel->childAtIndex(1));
        StackViewController *stack = stackController();
        m_lastSelect = selectedRow();
        stack->push(subController);
        return true;
    }
    GlobalPreferences::sharedGlobalPreferences()->setDfuDeacAlert(true);
    return GenericSubController::handleEvent(event);
}

HighlightCell *UsbInfoController::reusableCell(int index, int type) {
    assert(type == 2 || type == 1);
    if (type == 2) {
        assert(index >= 0 && index < k_maxSwitchCells);
        return &m_switchCells[index];
    }
    return &m_dfuLevel;
}

int UsbInfoController::reusableCellCount(int type) {
    assert(type == 2 || type == 1);
    if (type == 2) {
        return k_maxSwitchCells;
    }
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
            // mcell->setSubtitle(I18n::Message::USBDefaultLevel);
            mcell->setSubtitle(I18n::Message::USBDefaultLevelDesc);
        } else if (currentLevel == 1) {
            // mcell->setSubtitle(I18n::Message::USBLowLevel);
            mcell->setSubtitle(I18n::Message::USBLowLevelDesc);
        } else if (currentLevel == 2) {
            // mcell->setSubtitle(I18n::Message::USBParanoidLevel);
            mcell->setSubtitle(I18n::Message::USBParanoidLevelDesc);
        } else {
            // mcell->setSubtitle(I18n::Message::USBMegaParanoidLevel);
            mcell->setSubtitle(I18n::Message::USBMegaParanoidLevelDesc);
        }
    }
}

int UsbInfoController::typeAtLocation(int i, int j) {
    switch (j) {
        case 0:
            return 2;
        default:
            return 1;
    }
}

void UsbInfoController::didEnterResponderChain(Responder *previousFirstResponder) {
    m_contentView.reload();

    if (numberOfInfoLines() > 0) {
        I18n::Message infoMessages[] = {I18n::Message::USBE16_expl1, I18n::Message::USBE16_expl2, I18n::Message::USBE16_expl3};
        m_contentView.setMessages(infoMessages, numberOfInfoLines());
    }
}
}  
