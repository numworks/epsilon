#ifndef SOLVER_CONTROLLERS_COMPOUND_INTEREST_CONTROLLER_H
#define SOLVER_CONTROLLERS_COMPOUND_INTEREST_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "../gui/selectable_cell_list_controller.h"

namespace Solver {

// TODO Hugo : Add "Choose a parameter to solve" intro message

constexpr static int k_numberOfCompoundInterestCells = 5;

class CompoundInterestController : public SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage,
                                                     k_numberOfCompoundInterestCells> {
public:
  CompoundInterestController(Escher::StackViewController * parentResponder);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override { return I18n::translate(I18n::Message::CompoundInterest); }
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTitle; }
  int numberOfRows() const override { return k_numberOfCompoundInterestCells; }

  constexpr static int k_indexOfN = 0;
  constexpr static int k_indexOfRPct = 1;
  constexpr static int k_indexOfPV = 2;
  constexpr static int k_indexOfPmt = 3;
  constexpr static int k_indexOfFV = 4;
};

}  // namespace Solver

#endif /* SOLVER_CONTROLLERS_COMPOUND_INTEREST_CONTROLLER_H */
