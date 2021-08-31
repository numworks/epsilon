#include <escher/message_table_cell_with_chevron_and_expression.h>
#include <escher/metric.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithChevronAndExpression::MessageTableCellWithChevronAndExpression(I18n::Message message) :
  MessageTableCellWithChevron(message),
  m_subtitleView(KDFont::ALIGN_RIGHT, KDFont::ALIGN_CENTER, Palette::GrayDark)
{
}

void MessageTableCellWithChevronAndExpression::setHighlighted(bool highlight) {
  MessageTableCellWithChevron::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithChevronAndExpression::setLayout(Poincare::Layout layoutR) {
  m_subtitleView.setLayout(layoutR);
  reloadCell();
  layoutSubviews();
}

}
