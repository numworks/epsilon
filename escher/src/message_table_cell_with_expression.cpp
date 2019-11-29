#include <escher/message_table_cell_with_expression.h>
#include <escher/palette.h>

MessageTableCellWithExpression::MessageTableCellWithExpression(I18n::Message message, const KDFont * font) :
  MessageTableCell(message, font),
  m_subtitleView(1.0f, 0.5f, Palette::SecondaryText)
{
}

View * MessageTableCellWithExpression::accessoryView() const {
  return (View *)&m_subtitleView;
}

void MessageTableCellWithExpression::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::ListCellBackgroundSelected : Palette::ListCellBackground;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithExpression::setLayout(Poincare::Layout layout) {
  m_subtitleView.setLayout(layout);
  reloadCell();
  layoutSubviews();
}
