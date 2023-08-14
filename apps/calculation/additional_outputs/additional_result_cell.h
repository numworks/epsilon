#ifndef CALCULATION_ADDITIONAL_RESULT_CELL_H
#define CALCULATION_ADDITIONAL_RESULT_CELL_H

#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/palette.h>

#include "apps/i18n.h"
#include "scrollable_three_layouts_view.h"

namespace Calculation {

class AdditionalResultCell : public Escher::MenuCell<ScrollableThreeLayoutsView,
                                                     Escher::MessageTextView> {
 public:
  AdditionalResultCell(I18n::Message subLabelMessage = I18n::Message::Default)
      : Escher::MenuCell<ScrollableThreeLayoutsView, Escher::MessageTextView>(),
        m_highlightWholeCell(false) {}

  void setHighlighted(bool highlight) override {
    label()->evenOddCell()->setHighlighted(highlight);
    if (!m_highlightWholeCell) {
      return;
    }
    Escher::MenuCell<ScrollableThreeLayoutsView,
                     Escher::MessageTextView>::setHighlighted(highlight);
  }

  void setHighlightWholeCell(bool highlightWholeCell) {
    m_highlightWholeCell = highlightWholeCell;
    label()->setHighlightWholeCell(highlightWholeCell);
  };

 private:
  bool m_highlightWholeCell;
};

}  // namespace Calculation

#endif
