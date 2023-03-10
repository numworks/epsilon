#include <escher/message_table_cell_with_expression.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithExpression::MessageTableCellWithExpression(
    I18n::Message message)
    : MessageTableCell(message),
      m_subtitleView(
          KDGlyph::Format{{.glyphColor = Palette::GrayDark},
                          .horizontalAlignment = KDGlyph::k_alignRight}) {}

void MessageTableCellWithExpression::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  m_subtitleView.setBackgroundColor(defaultBackgroundColor());
}

void MessageTableCellWithExpression::setLayout(Poincare::Layout layout) {
  m_subtitleView.setLayout(layout);
  reloadCell();
}

}  // namespace Escher
