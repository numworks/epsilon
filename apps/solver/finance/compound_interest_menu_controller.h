#ifndef SOLVER_COMPOUND_INTEREST_MENU_CONTROLLER_H
#define SOLVER_COMPOUND_INTEREST_MENU_CONTROLLER_H

#include <apps/shared/selectable_cell_list_controller.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_text_view.h>
#include <escher/stack_view_controller.h>
#include <escher/table_view_with_top_and_bottom_views.h>
#include <escher/view_controller.h>
#include <ion/events.h>
#include "data.h"

namespace Solver {

constexpr int k_numberOfCompoundInterestCells = CompoundInterestData::k_numberOfUnknowns;

class CompoundInterestMenuController : public Shared::SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage,
                                                     k_numberOfCompoundInterestCells> {
public:
  CompoundInterestMenuController(Escher::StackViewController * parentResponder, Escher::ViewController * compoundInterestController, FinanceData * data);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override { return I18n::translate(I18n::Message::CompoundInterest); }
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTitle; }
  Escher::View * view() override { return &m_contentView; }
  int numberOfRows() const override { return k_numberOfCompoundInterestCells; }
private:
  int stackTitleStyleStep() const override { return 1; }
  CompoundInterestParameter paramaterAtIndex(int index) const;

  Escher::MessageTextView m_messageView;
  Escher::TableViewWithTopAndBottomViews m_contentView;
  Escher::ViewController * m_compoundInterestController;
  FinanceData * m_data;
};

}  // namespace Solver

#endif /* SOLVER_COMPOUND_INTEREST_MENU_CONTROLLER_H */
