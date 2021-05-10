#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/table_cell.h>
#include <escher/view_controller.h>

#include "../gui/page_controller.h"

namespace Probability {

// Controller
class MenuController : public SelectableListViewPage {
 public:
  MenuController(Escher::StackViewController * parentResponder, Escher::ViewController * distributionController,
                 Escher::ViewController * testController);
  int numberOfRows() const override { return k_numberOfCells; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  
 private:
  ViewController * m_distributionController;
  ViewController * m_testController;

  constexpr static int k_numberOfCells = 3;
  constexpr static int k_indexOfDistribution = 0;
  constexpr static int k_indexOfTest = 1;
  constexpr static int k_indexOfInterval = 2;
  Escher::MessageTableCellWithChevron m_cells[k_numberOfCells];
};

}  // namespace Probability

#endif /* MENU_CONTROLLER_H */
