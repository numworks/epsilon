#include <escher/expression_table_cell.h>
#include <escher/palette.h>
#include <assert.h>

ExpressionTableCell::ExpressionTableCell(Layout layout) :
  TableCell(layout),
  m_labelExpressionView(0.0f, 0.5f, Palette::PrimaryText, Palette::ListCellBackground)
{
}

View * ExpressionTableCell::labelView() const {
  return (View *)&m_labelExpressionView;
}

void ExpressionTableCell::setHighlighted(bool highlight) {
  TableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::ListCellBackgroundSelected : Palette::ListCellBackground;
  m_labelExpressionView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCell::setLayout(Poincare::Layout layout) {
  m_labelExpressionView.setLayout(layout);
  if (!layout.isUninitialized()) {
    layoutSubviews();
  }
}
