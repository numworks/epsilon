#include <escher/expression_table_cell_with_pointer.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

ExpressionTableCellWithPointer::ExpressionTableCellWithPointer(Responder * parentResponder, I18n::Message subLabelMessage) :
  ExpressionTableCell(parentResponder),
  m_subLabelView(KDFont::SmallFont, subLabelMessage, 0.0f, 0.5f, Palette::GrayDark, KDColorWhite)
{
}

View * ExpressionTableCellWithPointer::subLabelView() const {
  return (View *)&m_subLabelView;
}

void ExpressionTableCellWithPointer::setHighlighted(bool highlight) {
  ExpressionTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_subLabelView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCellWithPointer::setSubLabelMessage(I18n::Message text) {
  m_subLabelView.setMessage(text);
}

}
