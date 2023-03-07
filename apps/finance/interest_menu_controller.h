#ifndef FINANCE_SIMPLE_INTEREST_MENU_CONTROLLER_H
#define FINANCE_SIMPLE_INTEREST_MENU_CONTROLLER_H

#include <escher/list_view_data_source.h>
#include <escher/list_view_with_top_and_bottom_views.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>

#include "data.h"
#include "interest_controller.h"

namespace Finance {

constexpr int k_numberOfInterestCells = InterestData::k_maxNumberOfUnknowns;

class InterestMenuController
    : public Escher::SelectableCellListPage<
          Escher::MessageTableCellWithChevronAndMessage,
          k_numberOfInterestCells, Escher::MemoizedListViewDataSource> {
 public:
  InterestMenuController(Escher::StackViewController* parentResponder,
                         InterestController* interestController);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  Escher::View* view() override { return &m_contentView; }
  int numberOfRows() const override;

 private:
  uint8_t paramaterAtIndex(int index) const;

  Escher::MessageTextView m_messageView;
  Escher::ListViewWithTopAndBottomViews m_contentView;
  InterestController* m_interestController;
};

}  // namespace Finance

#endif
