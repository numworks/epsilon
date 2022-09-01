#ifndef FINANCE_MENU_CONTROLLER_H
#define FINANCE_MENU_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>
#include "data_controller.h"
#include "interest_menu_controller.h"

namespace Finance {

constexpr int k_numberOfFinanceCells = 2;

class MenuController : public Escher::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage, k_numberOfFinanceCells, Escher::RegularListViewDataSource>, public DataController {
public:
  MenuController(Escher::StackViewController * parentResponder,
                    InterestMenuController * interestMenuController,
                    Data * data);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;

  constexpr static int k_indexOfSimpleInterest = 0;
  constexpr static int k_indexOfCompoundInterest = 1;

private:
  InterestMenuController * m_interestMenuController;
};

}  // namespace Finance

#endif /* FINANCE_MENU_CONTROLLER_H */
