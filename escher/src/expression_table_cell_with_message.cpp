#include <escher/expression_table_cell_with_message.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

ExpressionTableCellWithMessage::ExpressionTableCellWithMessage(Responder * parentResponder, I18n::Message subLabelMessage) :
  ExpressionTableCell(parentResponder),
  m_subLabelView(KDFont::SmallFont, subLabelMessage, 0.0f, 0.5f, Palette::GrayDark, KDColorWhite)
{
}

void ExpressionTableCellWithMessage::setHighlighted(bool highlight) {
  ExpressionTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCellWithMessage::setSubLabelMessage(I18n::Message text) {
  m_subLabelView.setMessage(text);
}

}
