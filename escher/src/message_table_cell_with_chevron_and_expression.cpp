#include <escher/message_table_cell_with_chevron_and_expression.h>
#include <escher/metric.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithChevronAndExpression::
    MessageTableCellWithChevronAndExpression(I18n::Message message)
    : MessageTableCellWithChevron(message),
      m_subtitleView({{.glyphColor = Palette::GrayDark},
                      .horizontalAlignment = KDGlyph::k_alignRight}) {}

void MessageTableCellWithChevronAndExpression::setHighlighted(bool highlight) {
  MessageTableCellWithChevron::setHighlighted(highlight);
  m_subtitleView.setBackgroundColor(defaultBackgroundColor());
}

void MessageTableCellWithChevronAndExpression::setLayout(
    Poincare::Layout layoutR) {
  m_subtitleView.setLayout(layoutR);
  reloadCell();
  layoutSubviews();
}

}  // namespace Escher
