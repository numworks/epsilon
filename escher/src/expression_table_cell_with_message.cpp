#include <assert.h>
#include <escher/expression_table_cell_with_message.h>
#include <escher/palette.h>

namespace Escher {

ExpressionTableCellWithMessage::ExpressionTableCellWithMessage(
    Responder* parentResponder, I18n::Message subLabelMessage,
    KDFont::Size expressionFont)
    : ExpressionTableCell(parentResponder, expressionFont),
      m_subLabelView(KDFont::Size::Small, subLabelMessage,
                     KDContext::k_alignLeft, KDContext::k_alignCenter,
                     Palette::GrayDark, KDColorWhite) {}

void ExpressionTableCellWithMessage::setHighlighted(bool highlight) {
  ExpressionTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight ? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCellWithMessage::setSubLabelMessage(I18n::Message text) {
  m_subLabelView.setMessage(text);
}

}  // namespace Escher
