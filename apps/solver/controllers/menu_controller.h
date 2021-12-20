#ifndef SOLVER_CONTROLLERS_MENU_CONTROLLER_H
#define SOLVER_CONTROLLERS_MENU_CONTROLLER_H

// TODO Hugo : Factorize with probability ?

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/table_cell.h>

#include "list_controller.h"
#include "finance_menu_controller.h"
#include "../gui/centering_view.h"
#include "../gui/page_controller.h"
#include "../gui/subapp_cell.h"

namespace Solver {

// Controller
class MenuController : public SelectableListViewPage {
public:
  MenuController(Escher::StackViewController * parentResponder,
                 Escher::ButtonRowController * listController,
                 FinanceMenuController * financeMenuController);
  int numberOfRows() const override { return k_numberOfCells; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  Escher::View * view() override { return &m_contentView; }

  constexpr static int k_indexOfEquation = 0;
  constexpr static int k_indexOfFinance = 1;

private:
  int stackTitleStyleStep() const override { return 0; }
  Escher::ButtonRowController * m_listController;
  FinanceMenuController * m_financeMenuController;

  constexpr static int k_numberOfCells = 2;
  SubappCell m_cells[k_numberOfCells];

  CenteringView m_contentView;
};

}  // namespace Solver

#endif /* SOLVER_CONTROLLERS_MENU_CONTROLLER_H */
