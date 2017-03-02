#include <escher/expression_table_cell_with_pointer.h>
#include <escher/palette.h>
#include <assert.h>

ExpressionTableCellWithPointer::ExpressionTableCellWithPointer(char * accessoryText, Layout layout) :
  ExpressionTableCell(layout),
  m_accessoryView(PointerTextView(KDText::FontSize::Small, accessoryText, 0.0f, 0.5f, Palette::GreyDark, KDColorWhite))
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
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_accessoryView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCellWithPointer::setAccessoryText(const char * text) {
  m_accessoryView.setText(text);
}
