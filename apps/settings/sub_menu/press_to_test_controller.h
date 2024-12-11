#ifndef SETTINGS_PRESS_TO_TEST_CONTROLLER_H
#define SETTINGS_PRESS_TO_TEST_CONTROLLER_H

#include <apps/i18n.h>
#include <apps/shared/pop_up_controller.h>
#include <escher/button_cell.h>
#include <escher/list_with_top_and_bottom_controller.h>
#include <poincare/preferences.h>

#include "press_to_test_switch.h"

namespace Settings {

class PressToTestController : public Escher::ListWithTopAndBottomController {
 public:
  PressToTestController(Escher::Responder* parentResponder);
  const char* title() const override {
    return I18n::translate(I18n::Message::PressToTest);
  }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  int numberOfRows() const override;
  int typeAtRow(int row) const override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  KDCoordinate separatorBeforeRow(int row) override {
    return typeAtRow(row) == k_buttonCellType ? k_defaultRowSeparator : 0;
  }
  Poincare::ExamMode::PressToTestFlags getPressToTestParams();
  void resetController();

 private:
  /* Cell type */
  constexpr static int k_switchCellType = 0;
  constexpr static int k_buttonCellType = 1;
  /* Switch cells count */
  constexpr static int k_numberOfSwitchCells = 10;
  constexpr static int k_numberOfReusableSwitchCells = 7;

  // Switch Cells
  void setParamAtIndex(int index, bool value);
  bool getParamAtIndex(int index);
  void setMessages();

  Escher::MessageTextView m_topMessageView;
  Escher::MessageTextView m_bottomMessageView;
  PressToTestSwitch m_switchCells[k_numberOfReusableSwitchCells];
  Poincare::ExamMode::PressToTestFlags m_tempPressToTestParams;
  Escher::ButtonCell m_activateButton;
  Shared::MessagePopUpController m_confirmPopUpController;

  static I18n::Message LabelAtIndex(int index);
  static I18n::Message SubLabelAtIndex(int index);
};

}  // namespace Settings

#endif
