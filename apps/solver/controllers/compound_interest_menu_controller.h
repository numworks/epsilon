#ifndef SOLVER_CONTROLLERS_COMPOUND_INTEREST_MENU_CONTROLLER_H
#define SOLVER_CONTROLLERS_COMPOUND_INTEREST_MENU_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "compound_interest_controller.h"
#include "../gui/selectable_cell_list_controller.h"
#include "../gui/title_table_view.h"
#include "../model/data.h"

namespace Solver {

constexpr int k_numberOfCompoundInterestCells = CompoundInterestData::k_numberOfUnknowns;

class CompoundInterestMenuController : public SelectableCellListPage<Escher::MessageTableCellWithChevronAndMessage,
                                                     k_numberOfCompoundInterestCells> {
public:
  CompoundInterestMenuController(Escher::StackViewController * parentResponder, CompoundInterestController * compoundInterestController, FinanceData * data);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override { return I18n::translate(I18n::Message::CompoundInterest); }
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastTitle; }
  Escher::View * view() override { return &m_contentView; }
  int numberOfRows() const override { return k_numberOfCompoundInterestCells; }
private:
  int stackTitleStyleStep() const override { return 1; }
  CompoundInterestParameter paramaterAtIndex(int index) const;

  TitleTableView m_contentView;
  CompoundInterestController * m_compoundInterestController;
  FinanceData * m_data;
};

}  // namespace Solver

#endif /* SOLVER_CONTROLLERS_COMPOUND_INTEREST_MENU_CONTROLLER_H */
