#include <escher/even_odd_expression_cell.h>
#include <assert.h>
using namespace Poincare;

EvenOddExpressionCell::EvenOddExpressionCell(float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  EvenOddCell(),
  m_expressionView(horizontalAlignment, verticalAlignment, textColor, backgroundColor),
  m_leftMargin(0),
  m_rightMargin(0)
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

void EvenOddExpressionCell::setLayout(Layout layoutR) {
  m_expressionView.setLayout(layoutR);
}

void EvenOddExpressionCell::setBackgroundColor(KDColor backgroundColor) {
  m_expressionView.setBackgroundColor(backgroundColor);
}

void EvenOddExpressionCell::setTextColor(KDColor textColor) {
  m_expressionView.setTextColor(textColor);
}

KDSize EvenOddExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(m_leftMargin + expressionSize.width() + m_rightMargin, expressionSize.height());
}

void EvenOddExpressionCell::setLeftMargin(KDCoordinate margin) {
  m_leftMargin = margin;
  layoutSubviews();
}

void EvenOddExpressionCell::setRightMargin(KDCoordinate margin) {
  m_rightMargin = margin;
  layoutSubviews();
}

void EvenOddExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  // Color the margins
  ctx->fillRect(KDRect(0, 0, m_leftMargin, bounds().height()), backgroundColor());
  ctx->fillRect(KDRect(bounds().width() - m_rightMargin, 0, m_rightMargin, bounds().height()), backgroundColor());
}

int EvenOddExpressionCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionView;
}

void EvenOddExpressionCell::layoutSubviews(bool force) {
  m_expressionView.setFrame(KDRect(m_leftMargin, 0, bounds().width() - m_leftMargin - m_rightMargin, bounds().height()), force);
}
