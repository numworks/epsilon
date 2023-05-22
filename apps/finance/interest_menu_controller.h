#ifndef FINANCE_SIMPLE_INTEREST_MENU_CONTROLLER_H
#define FINANCE_SIMPLE_INTEREST_MENU_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_with_top_and_bottom_views.h>
#include <escher/stack_view_controller.h>
#include <ion/events.h>

#include "data.h"
#include "interest_controller.h"

namespace Finance {

using InterestMenuCell =
    Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                     Escher::ChevronView>;

class InterestMenuController
    : public Escher::SelectableListViewWithTopAndBottomViews {
 public:
  InterestMenuController(Escher::StackViewController* parentResponder,
                         InterestController* interestController);
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  int numberOfRows() const override;
  int reusableCellCount(int type) override {
    return InterestData::k_maxNumberOfUnknowns;
  }
  Escher::HighlightCell* reusableCell(int i, int type) override {
    assert(type == 0);
    assert(i >= 0 && i < InterestData::k_maxNumberOfUnknowns);
    return &m_cells[i];
  }

 private:
  uint8_t paramaterAtIndex(int index) const;

  Escher::MessageTextView m_messageView;
  InterestMenuCell m_cells[InterestData::k_maxNumberOfUnknowns];
  InterestController* m_interestController;
};

}  // namespace Finance

#endif
