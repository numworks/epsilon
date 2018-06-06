#include <escher/message_table_cell_with_expression.h>
#include <escher/palette.h>

MessageTableCellWithExpression::MessageTableCellWithExpression(I18n::Message message, KDText::FontSize size) :
  MessageTableCell(message, size),
  m_subtitleView(1.0f, 0.5f, Palette::GreyDark)
{
}

View * MessageTableCellWithExpression::subAccessoryView() const {
  return (View *)&m_subtitleView;
}

void MessageTableCellWithExpression::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithExpression::setExpressionLayout(Poincare::ExpressionLayout * expressionLayout) {
  m_subtitleView.setExpressionLayout(expressionLayout);
  reloadCell();
  layoutSubviews();
}
