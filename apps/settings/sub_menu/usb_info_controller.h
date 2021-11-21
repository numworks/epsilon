#ifndef SETTINGS_USB_INFO_CONTROLLER_H
#define SETTINGS_USB_INFO_CONTROLLER_H

#include "generic_sub_controller.h"
#include "preferences_controller.h"
#include "selectable_view_with_messages.h"
#include "usb_protection_level_controller.h"

namespace Settings {

class UsbInfoController : public GenericSubController {
   public:
    UsbInfoController(Responder* parentResponder);
    View* view() override { return &m_contentView; }
    bool handleEvent(Ion::Events::Event event) override;
    TELEMETRY_ID("UsbInfo");
    void didEnterResponderChain(Responder* previousFirstResponder) override;

    HighlightCell* reusableCell(int index, int type) override;
    int reusableCellCount(int type) override;
    void willDisplayCellForIndex(HighlightCell* cell, int index) override;

   private:
    static constexpr int k_numberOfExplanationMessages = 3;
    MessageTableCellWithSwitch m_switchCell;
    UsbProtectionLevelController m_usbProtectionLevelController;
    MessageTableCellWithChevronAndMessage m_dfuLevelCell;
};

}

#endif
