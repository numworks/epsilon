#include <escher/expression_table_cell.h>
#include <escher/palette.h>
#include <assert.h>

ExpressionTableCell::ExpressionTableCell(Layout layout) :
  TableCell(layout),
  m_labelExpressionView(0.0f, 0.5f, KDColorBlack, KDColorWhite)
{
}

View * ExpressionTableCell::labelView() const {
  return (View *)&m_labelExpressionView;
}

void ExpressionTableCell::setHighlighted(bool highlight) {
  TableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_labelExpressionView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCell::setExpression(Poincare::ExpressionLayout * expressionLayout) {
  m_labelExpressionView.setExpression(expressionLayout);
  layoutSubviews();
}
