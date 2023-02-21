#ifndef SETTINGS_TEST_MODE_CONTROLLER_H
#define SETTINGS_TEST_MODE_CONTROLLER_H

#include <escher/message_table_cell_with_chevron.h>

#include "generic_sub_controller.h"

namespace Settings {

class MainController;
class TestModeController : public GenericSubController {
 public:
  TestModeController(Escher::Responder* parentResponder,
                     MainController* mainController)
      : GenericSubController(parentResponder),
        m_mainController(mainController) {}
  TELEMETRY_ID("TestMode");
  KDCoordinate nonMemoizedRowHeight(int j) override;
  Escher::HighlightCell* reusableCell(int index, int type) override;
  bool handleEvent(Ion::Events::Event event) override;
  int reusableCellCount(int type) override { return k_numberOfCells; };
  void didBecomeFirstResponder() override;

 private:
  constexpr static int k_numberOfCells = 2;
  MainController* m_mainController;
  Escher::MessageTableCellWithChevron m_cells[k_numberOfCells];
};

}  // namespace Settings

#endif
