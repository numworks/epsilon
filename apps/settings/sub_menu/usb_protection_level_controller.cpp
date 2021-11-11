#include "usb_protection_level_controller.h"

#include <apps/i18n.h>
#include <assert.h>

#include "../../apps_container.h"
#include "../../global_preferences.h"

using namespace Poincare;
using namespace Shared;

namespace Settings {
UsbProtectionLevelController::UsbProtectionLevelController(Responder *parentResponder) : GenericSubController(parentResponder) {
    for (int i = 0; i < k_maxNumberOfCells; i++) {
        m_cell[i].setMessageFont(KDFont::LargeFont);
        m_cell[i].setAccessoryFont(KDFont::SmallFont);
    }
}

bool UsbProtectionLevelController::handleEvent(Ion::Events::Event event) {
    if (event == Ion::Events::OK || event == Ion::Events::EXE) {
        for (int i = 0; i < k_maxNumberOfCells; i++) {
            m_cell[i].setAccessoryText("");
        }
        if (m_messageTreeModel->childAtIndex(selectedRow())->label() == I18n::Message::USBLowLevel) {
            GlobalPreferences::sharedGlobalPreferences()->setDfuLevel(1);
        } else if (m_messageTreeModel->childAtIndex(selectedRow())->label() == I18n::Message::USBDefaultLevel) {
            GlobalPreferences::sharedGlobalPreferences()->setDfuLevel(0);
        } else if (m_messageTreeModel->childAtIndex(selectedRow())->label() == I18n::Message::USBParanoidLevel) {
            GlobalPreferences::sharedGlobalPreferences()->setDfuLevel(2);
        } else if (m_messageTreeModel->childAtIndex(selectedRow())->label() == I18n::Message::USBMegaParanoidLevel) {
            GlobalPreferences::sharedGlobalPreferences()->setDfuLevel(3);
        }

        StackViewController * stack = stackController();
        stack->pop();
        return true;
    } else {
        return GenericSubController::handleEvent(event);
    }
}

HighlightCell *UsbProtectionLevelController::reusableCell(int index, int type) {
    assert(index >= 0 && index < k_maxNumberOfCells);
    return &m_cell[index];
}

int UsbProtectionLevelController::reusableCellCount(int type) {
    return k_maxNumberOfCells;
}

void UsbProtectionLevelController::willDisplayCellForIndex(HighlightCell *cell, int index) {
    GenericSubController::willDisplayCellForIndex(cell, index);
    I18n::Message childLabel = m_messageTreeModel->childAtIndex(index)->label();
    MessageTableCellWithBuffer *messageComp = (MessageTableCellWithBuffer *)cell;

    int currentLevel = GlobalPreferences::sharedGlobalPreferences()->getDfuLevel();

    if (childLabel == I18n::Message::USBLowLevel && currentLevel == 1) {
        // messageComp->setTextColor(Palette::Green);
        messageComp->setAccessoryText("√");
    } else if (childLabel == I18n::Message::USBDefaultLevel && currentLevel == 0) {
        // messageComp->setTextColor(Palette::Green);
        messageComp->setAccessoryText("√");
    } else if (childLabel == I18n::Message::USBParanoidLevel && currentLevel == 2) {
        // messageComp->setTextColor(Palette::Green);
        messageComp->setAccessoryText("√");
    } else if (childLabel == I18n::Message::USBMegaParanoidLevel && currentLevel == 3) {
        // messageComp->setTextColor(Palette::Green);
        messageComp->setAccessoryText("√");
    }
}

int UsbProtectionLevelController::typeAtLocation(int i, int j) {
    return 0;
}
}  // namespace Settings