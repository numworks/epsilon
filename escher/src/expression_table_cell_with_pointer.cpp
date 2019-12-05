#include <escher/expression_table_cell_with_pointer.h>
#include <escher/palette.h>
#include <assert.h>

ExpressionTableCellWithPointer::ExpressionTableCellWithPointer(I18n::Message accessoryMessage, Layout layout) :
  ExpressionTableCell(layout),
  m_accessoryView(KDFont::SmallFont, accessoryMessage, 0.0f, 0.5f, Palette::SecondaryText, Palette::ListCellBackground)
{
  if (layout == Layout::Horizontal) {
    m_accessoryView.setAlignment(1.0f, 0.5f);
  }
}

View * ExpressionTableCellWithPointer::accessoryView() const {
  return (View *)&m_accessoryView;
}

void ExpressionTableCellWithPointer::setHighlighted(bool highlight) {
  ExpressionTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::ListCellBackgroundSelected : Palette::ListCellBackground;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCellWithPointer::setAccessoryMessage(I18n::Message text) {
  m_accessoryView.setMessage(text);
}
