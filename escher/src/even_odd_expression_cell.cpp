#include <escher/even_odd_expression_cell.h>
#include <assert.h>
using namespace Poincare;

EvenOddExpressionCell::EvenOddExpressionCell(float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor) :
  EvenOddCell(),
  m_expressionView(horizontalAlignment, verticalAlignment, textColor, backgroundColor)
{
}

void EvenOddExpressionCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_expressionView.setBackgroundColor(backgroundColor());
}

void EvenOddExpressionCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_expressionView.setBackgroundColor(backgroundColor());
}

KDSize EvenOddExpressionCell::minimalSizeForOptimalDisplay() const {
  return m_expressionView.minimalSizeForOptimalDisplay();
}

int EvenOddExpressionCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionView;
}

void EvenOddExpressionCell::layoutSubviews() {
  m_expressionView.setFrame(bounds());
}
