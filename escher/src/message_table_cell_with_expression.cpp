#include <escher/message_table_cell_with_expression.h>
#include <escher/palette.h>

namespace Escher {

MessageTableCellWithExpression::MessageTableCellWithExpression(I18n::Message message) :
  MessageTableCell(message),
  m_subtitleView(1.0f, 0.5f, Palette::GrayDark)
{
}

View * MessageTableCellWithExpression::subLabelView() const {
  return (View *)&m_subtitleView;
}

void MessageTableCellWithExpression::setHighlighted(bool highlight) {
  MessageTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subtitleView.setBackgroundColor(backgroundColor);
}

void MessageTableCellWithExpression::setLayout(Poincare::Layout layout) {
  m_subtitleView.setLayout(layout);
  reloadCell();
}

}
