#include <escher/expression_table_cell_with_expression.h>
#include <escher/container.h>
#include <escher/palette.h>
#include <assert.h>

namespace Escher {

ExpressionTableCellWithExpression::ExpressionTableCellWithExpression(Responder * parentResponder) :
  ExpressionTableCell(parentResponder),
  m_subLabelExpressionView(this, 0, 0, 1.0f, 0.5f, Palette::GrayDark, KDColorWhite)
{}

void ExpressionTableCellWithExpression::setHighlighted(bool highlight) {
  ExpressionTableCell::setHighlighted(highlight);
  KDColor backgroundColor = highlight? Palette::Select : KDColorWhite;
  m_subLabelExpressionView.setBackgroundColor(backgroundColor);
}

void ExpressionTableCellWithExpression::setSubLabelLayout(Poincare::Layout l) {
  m_subLabelExpressionView.setLayout(l);
  layoutSubviews();
}

void ExpressionTableCellWithExpression::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_subLabelExpressionView);
}

}
