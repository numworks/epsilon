#include <escher/message_table_cell_with_chevron_and_expression.h>
#include <escher/metric.h>
#include <escher/palette.h>

MessageTableCellWithChevronAndExpression::MessageTableCellWithChevronAndExpression(I18n::Message message, const KDFont * font) :
  MessageTableCellWithChevron(message, font),
  m_subtitleView(1.0f, 0.5f, Palette::GrayDark)
{
  m_subtitleView.setHorizontalMargin(Metric::ExpressionViewHorizontalMargin);
}

View * MessageTableCellWithChevronAndExpression::subAccessoryView() const {
  return (View *)&m_subtitleView;
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
