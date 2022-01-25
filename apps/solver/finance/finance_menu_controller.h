#ifndef SOLVER_FINANCE_MENU_CONTROLLER_H
#define SOLVER_FINANCE_MENU_CONTROLLER_H

#include <apps/shared/selectable_cell_list_controller.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>
#include "data.h"
#include "interest_menu_controller.h"

namespace Solver {

constexpr int k_numberOfFinanceCells = 2;

class FinanceMenuController : public Shared::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage,
                                                     k_numberOfFinanceCells> {
public:
  FinanceMenuController(Escher::StackViewController * parentResponder,
                    InterestMenuController * interestMenuController,
                    FinanceData * financeData);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override;
  int numberOfRows() const override { return k_numberOfFinanceCells; }

  constexpr static int k_indexOfSimpleInterest = 0;
  constexpr static int k_indexOfCompoundInterest = 1;

private:
  InterestMenuController * m_interestMenuController;
  FinanceData * m_financeData;
};

}  // namespace Solver

#endif /* SOLVER_FINANCE_MENU_CONTROLLER_H */
