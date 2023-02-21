#include <escher/message_table_cell_with_expression.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithExpression::MessageTableCellWithExpression(
    I18n::Message message)
    : MessageTableCell(message),
      m_subtitleView(KDContext::k_alignRight, KDContext::k_alignCenter,
                     Palette::GrayDark) {}

void MessageTableCellWithExpression::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  m_subtitleView.setBackgroundColor(defaultBackgroundColor());
}

void MessageTableCellWithExpression::setLayout(Poincare::Layout layout) {
  m_subtitleView.setLayout(layout);
  reloadCell();
}

}  // namespace Escher
