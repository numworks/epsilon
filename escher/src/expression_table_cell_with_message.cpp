#include <escher/expression_table_cell_with_message.h>

namespace Escher {

ExpressionTableCellWithMessage::ExpressionTableCellWithMessage(Responder * parentResponder, I18n::Message message) :
  ExpressionTableCell(parentResponder),
  m_subLabelView(KDFont::SmallFont, (I18n::Message)0, 0.0f, 0.5f, Palette::GrayDark)
{}

View * ExpressionTableCellWithMessage::subLabelView() const {
  if (strlen(m_subLabelView.text()) == 0) {
    return nullptr;
  }
  return (View *)&m_subLabelView;
}

void ExpressionTableCellWithMessage::setHighlighted(bool highlight) {
  ExpressionTableCell::setHighlighted(highlight);
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCellWithMessage::setSubLabelMessage(I18n::Message textBody) {
  m_subLabelView.setMessage(textBody);
  reloadCell();
}

}