#include <string.h>
#include <assert.h>
#include "fraction_layout.h"

namespace Poincare {

FractionLayout::FractionLayout(ExpressionLayout * numerator_layout, ExpressionLayout * denominator_layout) :
ExpressionLayout(), m_numerator_layout(numerator_layout), m_denominator_layout(denominator_layout) {
  m_numerator_layout->setParent(this);
  m_denominator_layout->setParent(this);
  m_baseline = m_numerator_layout->size().height()
    + k_fractionLineMargin + k_fractionLineHeight;
}

FractionLayout::~FractionLayout() {
  delete m_denominator_layout;
  delete m_numerator_layout;
}

void FractionLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  KDCoordinate fractionLineY = p.y() + m_numerator_layout->size().height() + k_fractionLineMargin;
  ctx->fillRect(KDRect(p.x()+k_fractionBorderMargin, fractionLineY, size().width()-2*k_fractionBorderMargin, 1), expressionColor);
}

KDSize FractionLayout::computeSize() {
  KDCoordinate width = max(m_numerator_layout->size().width(), m_denominator_layout->size().width())
    + 2*k_fractionBorderLength+2*k_fractionBorderMargin;
  KDCoordinate height = m_numerator_layout->size().height()
    + k_fractionLineMargin + k_fractionLineHeight + k_fractionLineMargin
    + m_denominator_layout->size().height();
  return KDSize(width, height);
}

ExpressionLayout * FractionLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_numerator_layout;
    case 1:
      return m_denominator_layout;
    default:
      return nullptr;
  }
}

KDPoint FractionLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_numerator_layout) {
    x = (KDCoordinate)((size().width() - m_numerator_layout->size().width())/2);
  } else if (child == m_denominator_layout) {
    x = (KDCoordinate)((size().width() - m_denominator_layout->size().width())/2);
    y = (KDCoordinate)(m_numerator_layout->size().height() + 2*k_fractionLineMargin + k_fractionLineHeight);
  } else {
    assert(false); // Should not happen
  }
  return KDPoint(x, y);
}

}
