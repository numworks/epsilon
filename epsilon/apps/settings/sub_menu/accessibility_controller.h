#pragma once

#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/switch_view.h>

#include "generic_sub_controller.h"

namespace Settings {

class AccessibilityController : public GenericSubController {
 public:
  AccessibilityController(Escher::Responder* parentResponder);
  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

 private:
  constexpr static int k_totalNumberOfCell = 2;
  constexpr static int k_contrastRow = 0;
  constexpr static int k_magnifyRow = 1;

  using SwitchCell =
      Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                       Escher::SwitchView>;

  SwitchCell m_switchCells[k_totalNumberOfCell];
};

}  // namespace Settings
