#include <escher/expression_table_cell_with_pointer.h>
#include <escher/palette.h>
#include <assert.h>

ExpressionTableCellWithPointer::ExpressionTableCellWithPointer(Responder * parentResponder, I18n::Message accessoryMessage, Layout layout) :
  ExpressionTableCell(parentResponder, layout),
  m_accessoryView(KDFont::SmallFont, accessoryMessage, 0.0f, 0.5f, Palette::GreyDark, KDColorWhite)
{
  if (layout != Layout::Vertical) {
    m_accessoryView.setAlignment(1.0f, 0.5f);
  }
}

View * ExpressionTableCellWithPointer::accessoryView() const {
  return (View *)&m_accessoryView;
}

void ExpressionTableCellWithPointer::setHighlighted(bool highlight) {
  ExpressionTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCellWithPointer::setAccessoryMessage(I18n::Message text) {
  m_accessoryView.setMessage(text);
}
