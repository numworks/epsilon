#ifndef SETTINGS_usb_protection_controller_H
#define SETTINGS_usb_protection_controller_H

#include "generic_sub_controller.h"
#include "preferences_controller.h"
#include "selectable_view_with_messages.h"

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
  PreferencesController m_usbProtectionLevelController;
  SelectableViewWithMessages m_contentView;
  MessageTableCellWithSwitch m_switchCell;
  MessageTableCellWithChevronAndMessage m_dfuLevelCell;
};

}

#endif
