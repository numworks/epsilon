#ifndef FINANCE_SIMPLE_INTEREST_MENU_CONTROLLER_H
#define FINANCE_SIMPLE_INTEREST_MENU_CONTROLLER_H

#include <escher/chevron_view.h>
#include <escher/list_with_top_and_bottom_controller.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>

#include "data.h"
#include "parameters_controller.h"

namespace Finance {

using InterestMenuCell =
    Escher::MenuCell<Escher::MessageTextView, Escher::MessageTextView,
                     Escher::ChevronView>;

class InterestMenuController : public Escher::ListWithTopAndBottomController {
 public:
  InterestMenuController(Escher::StackViewController* parentResponder,
                         ParametersController* interestController);
  void viewWillAppear() override;
  bool handleEvent(Ion::Events::Event e) override;
  const char* title() const override;
  ViewController::TitlesDisplay titlesDisplay() const override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  int numberOfRows() const override;
  int reusableCellCount(int type) const override {
    return InterestData::k_maxNumberOfUnknowns;
  }
  Escher::HighlightCell* reusableCell(int i, int type) override {
    assert(type == 0);
    assert(i >= 0 && i < InterestData::k_maxNumberOfUnknowns);
    return &m_cells[i];
  }
  KDCoordinate nonMemoizedRowHeight(int row) override;

 private:
  uint8_t parameterAtIndex(int index) const;

  Escher::MessageTextView m_messageView;
  InterestMenuCell m_cells[InterestData::k_maxNumberOfUnknowns];
  ParametersController* m_parametersController;
};

}  // namespace Finance

#endif
