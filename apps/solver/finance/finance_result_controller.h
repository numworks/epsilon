#ifndef SOLVER_FINANCE_RESULT_CONTROLLER_H
#define SOLVER_FINANCE_RESULT_CONTROLLER_H

#include <apps/shared/selectable_cell_list_controller.h>
#include <escher/message_table_cell_with_message_with_buffer.h>
#include <escher/message_text_view.h>
#include <escher/stack_view_controller.h>
#include <escher/table_view_with_top_and_bottom_views.h>
#include <ion/events.h>
#include "data.h"

namespace Solver {

// TODO Hugo : Factorize with compound interest result controller

constexpr int k_numberOfResultCells = 1;

class FinanceResultController : public Shared::SelectableCellListPage<Escher::MessageTableCellWithMessageWithBuffer,
                                                     k_numberOfResultCells> {
public:
  FinanceResultController(Escher::StackViewController * parentResponder, FinanceData * data);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char * title() override;
  ViewController::TitlesDisplay titlesDisplay() override { return ViewController::TitlesDisplay::DisplayLastAndThirdToLast; }
  Escher::View * view() override { return &m_contentView; }
  int numberOfRows() const override { return k_numberOfResultCells; }
private:
  SimpleInterestData * simpleInterestData() const { assert(m_data->isSimpleInterest); return &(m_data->m_data.m_simpleInterestData); }
  CompoundInterestData * compoundInterestData() const { assert(!m_data->isSimpleInterest); return &(m_data->m_data.m_compoundInterestData); }
  int stackTitleStyleStep() const override { return 1; }

  static constexpr int k_titleBufferSize = 1 + Ion::Display::Width / 7; // KDFont::SmallFont->glyphSize().width() = 7
  char m_titleBuffer[k_titleBufferSize];

  Escher::MessageTextView m_messageView;
  Escher::TableViewWithTopAndBottomViews m_contentView;
  FinanceData * m_data;
};

}  // namespace Solver

#endif /* SOLVER_FINANCE_RESULT_CONTROLLER_H */
