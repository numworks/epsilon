#ifndef SOLVER_CONTROLLERS_SIMPLE_INTEREST_MENU_CONTROLLER_H
#define SOLVER_CONTROLLERS_SIMPLE_INTEREST_MENU_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "simple_interest_controller.h"
#include "../gui/selectable_cell_list_controller.h"
#include "../model/data.h"

namespace Solver {

// TODO Hugo : Add "Choose a parameter to solve" intro message

constexpr static int k_numberOfSimpleInterestCells = 4;

class SimpleInterestMenuController : public SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage,
                                                     k_numberOfSimpleInterestCells> {
public:
  SimpleInterestMenuController(Escher::StackViewController * parentResponder, SimpleInterestController * simpleInterestController, FinanceData * data);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override { return I18n::translate(I18n::Message::SimpleInterest); }
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTitle; }
  int numberOfRows() const override { return k_numberOfSimpleInterestCells; }
private:
  int stackTitleStyleStep() const override { return 1; }
  SimpleInterestParameter paramaterAtIndex(int index) const;
  SimpleInterestController * m_simpleInterestController;
  FinanceData * m_data;
};

}  // namespace Solver

#endif /* SOLVER_CONTROLLERS_SIMPLE_INTEREST_MENU_CONTROLLER_H */
