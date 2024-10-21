#ifndef SETTINGS_SCREEN_TIMEOUT_CONTROLLER_H
#define SETTINGS_SCREEN_TIMEOUT_CONTROLLER_H

#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "generic_sub_controller.h"

namespace Settings {

class ScreenTimeoutController : public GenericSubController {
 public:
  ScreenTimeoutController(Escher::Responder* parentResponder)
      : GenericSubController(parentResponder) {}

  bool handleEvent(Ion::Events::Event event) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  int reusableCellCount(int type) const override;
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;
  KDCoordinate nonMemoizedRowHeight(int row) override;
  void viewWillAppear() override;

  constexpr static int k_totalNumberOfCell = 4;

 protected:
  int initialSelectedRow() const override;

 private:
  Escher::MenuCell<Escher::MessageTextView, Escher::LayoutView>
      m_cells[k_totalNumberOfCell];
};

}  // namespace Settings

#endif
