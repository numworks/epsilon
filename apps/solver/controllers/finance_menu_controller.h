#ifndef SOLVER_CONTROLLERS_FINANCE_MENU_CONTROLLER_H
#define SOLVER_CONTROLLERS_FINANCE_MENU_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "../gui/selectable_cell_list_controller.h"
#include "simple_interest_menu_controller.h"
#include "compound_interest_menu_controller.h"

namespace Solver {

constexpr static int k_numberOfFinanceCells = 2;

class FinanceMenuController : public SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage,
                                                     k_numberOfFinanceCells> {
public:
  FinanceMenuController(Escher::StackViewController * parentResponder,
                    SimpleInterestMenuController * simpleInterestMenuController,
                    CompoundInterestMenuController * compoundInterestMenuController);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override;
  int numberOfRows() const override { return k_numberOfFinanceCells; }

  constexpr static int k_indexOfSimpleIntereset = 0;
  constexpr static int k_indexOfCompoundInterest = 1;

private:
  int stackTitleStyleStep() const override { return 0; }

  SimpleInterestMenuController * m_simpleInterestMenuController;
  CompoundInterestMenuController * m_compoundInterestMenuController;
};

}  // namespace Solver

#endif /* SOLVER_CONTROLLERS_FINANCE_MENU_CONTROLLER_H */
