#include <escher/even_odd_editable_expression_cell.h>
#include <assert.h>
using namespace Poincare;

namespace Escher {

EvenOddEditableExpressionCell::EvenOddEditableExpressionCell(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, LayoutFieldDelegate * layoutDelegate, float horizontalAlignment, float verticalAlignment,
    KDColor textColor, KDColor backgroundColor, KDFont::Size font) :
  EvenOddCell(),
  m_expressionField(parentResponder, inputEventHandlerDelegate, layoutDelegate, horizontalAlignment, verticalAlignment),
  m_leftMargin(0),
  m_rightMargin(0)
{
}

void EvenOddEditableExpressionCell::setLayout(Layout layoutR) {
  m_expressionField.setLayout(layoutR);
}

void EvenOddEditableExpressionCell::setTextColor(KDColor textColor) {
  m_expressionField.expressionView()->setTextColor(textColor);
}

KDSize EvenOddEditableExpressionCell::minimalSizeForOptimalDisplay() const {
  KDSize expressionSize = m_expressionField.minimalSizeForOptimalDisplay();
  return KDSize(m_leftMargin + expressionSize.width() + m_rightMargin, expressionSize.height());
}

void EvenOddEditableExpressionCell::setLeftMargin(KDCoordinate margin) {
  m_leftMargin = margin;
  layoutSubviews();
}

void EvenOddEditableExpressionCell::setRightMargin(KDCoordinate margin) {
  m_rightMargin = margin;
  layoutSubviews();
}

void EvenOddEditableExpressionCell::drawRect(KDContext * ctx, KDRect rect) const {
  // Color the margins
  ctx->fillRect(KDRect(0, 0, m_leftMargin, bounds().height()), backgroundColor());
  ctx->fillRect(KDRect(bounds().width() - m_rightMargin, 0, m_rightMargin, bounds().height()), backgroundColor());
}

int EvenOddEditableExpressionCell::numberOfSubviews() const {
  return 1;
}

View * EvenOddEditableExpressionCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_expressionField;
}

void EvenOddEditableExpressionCell::layoutSubviews(bool force) {
  m_expressionField.setFrame(KDRect(m_leftMargin, 0, bounds().width() - m_leftMargin - m_rightMargin, bounds().height()), force);
}

}
