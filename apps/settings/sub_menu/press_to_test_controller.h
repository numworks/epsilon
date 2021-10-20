#ifndef SETTINGS_PRESS_TO_TEST_CONTROLLER_H
#define SETTINGS_PRESS_TO_TEST_CONTROLLER_H

#include <escher/message_table_cell_with_message_with_switch.h>
#include <apps/shared/button_with_separator.h>
#include "generic_sub_controller.h"
#include "selectable_view_with_messages.h"
#include "../../global_preferences.h"

namespace Settings {

class PressToTestController : public GenericSubController {
public:
  PressToTestController(Escher::Responder * parentResponder);
  const char * title() override { return I18n::translate(I18n::Message::PressToTest); }
  bool handleEvent(Ion::Events::Event event) override;
  TELEMETRY_ID("PressToTest");
  void didEnterResponderChain(Escher::Responder * previousFirstResponder) override;
  int numberOfRows() const override;
  int typeAtIndex(int index) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  // Override GenericSubController implementation
  KDCoordinate nonMemoizedRowHeight(int j) override {
    // TODO Hugo : Improve this
    if (typeAtIndex(j) == k_buttonCellType) {
      return heightForCellAtIndex(&m_activateButton, j);
    }
    Escher::MessageTableCellWithMessageWithSwitch tempCell;
    return heightForCellAtIndex(&tempCell, j);
  }
private:
  static constexpr int k_numberOfSwitchCells = 7;
  static constexpr int k_numberOfReusableSwitchCells = 6;
  static constexpr int k_switchCellType = 0;
  static constexpr int k_buttonCellType = 1;
  // TODO Hugo : Add instructions and deactivation messages

  // Switch Cells
  // TODO Hugo : Handle change of accessory if exam mode is activated
  Escher::MessageTableCellWithMessageWithSwitch m_switchCells[k_numberOfReusableSwitchCells];
  void initSwitches();

  bool m_tempSwitchState[k_numberOfSwitchCells];
  Shared::ButtonWithSeparator m_activateButton;
  static I18n::Message LabelAtIndex(int index);
  static I18n::Message SubLabelAtIndex(int index);
};

}

#endif
