#ifndef SETTINGS_EXTERNAL_CONTROLLER_H
#define SETTINGS_EXTERNAL_CONTROLLER_H

#include "generic_sub_controller.h"
#include "preferences_controller.h"
#include "selectable_view_with_messages.h"

namespace Settings {

class ExternalController : public GenericSubController {
public:
  ExternalController(Responder* parentResponder);
  View* view() override { return &m_contentView; }
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("ExternalSettings");
  void didEnterResponderChain(Responder* previousFirstResponder) override;
  HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell* cell, int index) override;
private:
  static constexpr int k_numberOfExplanationMessages = 3;
  SelectableViewWithMessages m_contentView;
  MessageTableCellWithSwitch m_writeSwitchCell;
  MessageTableCellWithSwitch m_enabledSwitchCell;
};

}

#endif
