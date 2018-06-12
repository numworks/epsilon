#include <escher/even_odd_expression_cell.h>
#include <assert.h>
using namespace Poincare;

EvenOddExpressionCell::EvenOddExpressionCell(float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  EvenOddCell(),
  m_expressionView(horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_margin(0)
{
}

void EvenOddExpressionCell::setHighlighted(bool highlight) {
  if (highlight != EvenOddCell::isHighlighted()) {
    EvenOddCell::setHighlighted(highlight);
    m_expressionView.setBackgroundColor(backgroundColor());
  }
}

void EvenOddExpressionCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_expressionView.setBackgroundColor(backgroundColor());
}

void EvenOddExpressionCell::setExpressionLayout(ExpressionLayout * expressionLayout) {
  m_expressionView.setExpressionLayout(expressionLayout);
}

void EvenOddExpressionCell::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
}

void EvenOddExpressionCell::setTextColor(KDColor textColor) {
  m_expressionView.setTextColor(textColor);
}

KDSize EvenOddExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(m_margin+expressionSize.width(), expressionSize.height());
}

void EvenOddExpressionCell::setMargin(KDCoordinate margin) {
  m_margin = margin;
  layoutSubviews();
}

void EvenOddExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  // Color the margin
  ctx->fillRect(KDRect(0, 0, m_margin, bounds().height()), backgroundColor());
}

int EvenOddExpressionCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionView;
}

void EvenOddExpressionCell::layoutSubviews() {
  m_expressionView.setFrame(KDRect(m_margin, 0, bounds().width()-m_margin, bounds().height()));
}
