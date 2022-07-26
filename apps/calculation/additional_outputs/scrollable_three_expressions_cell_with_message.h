#ifndef CALCULATION_SCROLLABLE_THREE_EXPRESSIONS_WITH_MESSAGE_CELL_H
#define CALCULATION_SCROLLABLE_THREE_EXPRESSIONS_WITH_MESSAGE_CELL_H

#include "apps/i18n.h"
#include "scrollable_three_expressions_cell.h"
#include <escher/message_text_view.h>
#include <escher/table_cell.h>
#include <escher/palette.h>

namespace Calculation {

class ScrollableThreeExpressionsCellWithMessage : public ScrollableThreeExpressionsCell {
public:
  static KDCoordinate Height(Calculation * calculation, Poincare::Context * context);
  ScrollableThreeExpressionsCellWithMessage(I18n::Message subLabelMessage = I18n::Message::Default) :
    ScrollableThreeExpressionsCell(), m_subLabelView(KDFont::Size::Small, subLabelMessage, KDContext::k_alignLeft, KDContext::k_alignCenter, Escher::Palette::GrayDark, KDColorWhite) {
  }

  // Table Cell
  const View * subLabelView() const override { return &m_subLabelView; }
  void setSubLabelMessage(I18n::Message text) { m_subLabelView.setMessage(text); }
private:
  Escher::MessageTextView m_subLabelView;
};

}

#endif
