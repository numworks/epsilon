#include <assert.h>
#include <escher/even_odd_expression_cell.h>
using namespace Poincare;

namespace Escher {

EvenOddExpressionCell::EvenOddExpressionCell(float horizontalAlignment,
                                             float verticalAlignment,
                                             KDColor textColor,
                                             KDColor backgroundColor,
                                             KDFont::Size font)
    : EvenOddCell(),
      m_expressionView(horizontalAlignment, verticalAlignment, textColor,
                       backgroundColor, font),
      m_leftMargin(0),
      m_rightMargin(0) {}

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
  return KDSize(m_leftMargin + expressionSize.width() + m_rightMargin,
                expressionSize.height());
}

void EvenOddExpressionCell::setLeftMargin(KDCoordinate margin) {
  m_leftMargin = margin;
  layoutSubviews();
}

void EvenOddExpressionCell::setRightMargin(KDCoordinate margin) {
  m_rightMargin = margin;
  layoutSubviews();
}

void EvenOddExpressionCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Color the margins
  ctx->fillRect(KDRect(0, 0, m_leftMargin, bounds().height()),
                backgroundColor());
  ctx->fillRect(KDRect(bounds().width() - m_rightMargin, 0, m_rightMargin,
                       bounds().height()),
                backgroundColor());
}

void EvenOddExpressionCell::updateSubviewsBackgroundAfterChangingState() {
  m_expressionView.setBackgroundColor(backgroundColor());
}

int EvenOddExpressionCell::numberOfSubviews() const { return 1; }

View* EvenOddExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionView;
}

void EvenOddExpressionCell::layoutSubviews(bool force) {
  setChildFrame(
      &m_expressionView,
      KDRect(m_leftMargin, 0, bounds().width() - m_leftMargin - m_rightMargin,
             bounds().height()),
      force);
}

}  // namespace Escher
