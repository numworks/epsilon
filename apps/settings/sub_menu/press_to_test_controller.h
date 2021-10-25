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
  GlobalPreferences::PressToTestParams getPressToTestParams();
private:
  /* Cell type */
  static constexpr int k_switchCellType = 0;
  static constexpr int k_buttonCellType = 1;
  /* Switch cells index */
  static constexpr int k_equationSolverIndex = 0;
  static constexpr int k_inequalityGraphingIndex = k_equationSolverIndex + 1;
  static constexpr int k_implicitPlotsIndex = k_inequalityGraphingIndex + 1;
  static constexpr int k_statDiagnosticIndex = k_implicitPlotsIndex + 1;
  static constexpr int k_vectorsIndex = k_statDiagnosticIndex + 1;
  static constexpr int k_basedLogarithmIndex = k_vectorsIndex + 1;
  static constexpr int k_sumIndex = k_basedLogarithmIndex + 1;
  /* Switch cells count */
  static constexpr int k_numberOfSwitchCells = k_sumIndex + 1;
  // At most 6 switch cells fit in the screen.
  static constexpr int k_numberOfReusableSwitchCells = 6;
  // TODO Hugo : Add instructions and deactivation messages

  // Switch Cells
  // TODO Hugo : Handle change of accessory if exam mode is activated
  void resetSwitches();
  void setParamAtIndex(int index, bool value);
  bool getParamAtIndex(int index);

  Escher::MessageTableCellWithMessageWithSwitch m_switchCells[k_numberOfReusableSwitchCells];
  GlobalPreferences::PressToTestParams m_tempPressToTestParams;
  Shared::ButtonWithSeparator m_activateButton;

  static I18n::Message LabelAtIndex(int index);
  static I18n::Message SubLabelAtIndex(int index);
};

}

#endif
