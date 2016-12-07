#include <string.h>
#include <assert.h>
#include "exponent_layout.h"

ExponentLayout::ExponentLayout(ExpressionLayout * base_layout, ExpressionLayout * exponent_layout) :
ExpressionLayout(), m_base_layout(base_layout), m_exponent_layout(exponent_layout) {
  m_base_layout->setParent(this);
  m_exponent_layout->setParent(this);
  m_baseline = m_exponent_layout->baseline() + m_base_layout->baseline() - k_exponentHeight;
}

ExponentLayout::~ExponentLayout() {
  delete m_exponent_layout;
  delete m_base_layout;
}

void ExponentLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  // There is nothing to draw for a power, only the position of the children matters
}

KDSize ExponentLayout::computeSize() {
  KDSize exponent_size = m_exponent_layout->size();
  KDSize base_size = m_base_layout->size();
  return KDSize(
      base_size.height() + exponent_size.height() - k_exponentHeight,
      base_size.width() + exponent_size.width()
      );
}

ExpressionLayout * ExponentLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_base_layout;
    case 1:
      return m_exponent_layout;
    default:
      return nullptr;
  }
}

KDPoint ExponentLayout::positionOfChild(ExpressionLayout * child) {
  KDCoordinate x = 0;
  KDCoordinate y = 0;
  if (child == m_base_layout) {
    x = 0;
    y = m_exponent_layout->baseline() - k_exponentHeight;
  } else if (child == m_exponent_layout) {
    x = m_base_layout->size().width();
    y = 0;
  } else {
    assert(false); // Should not happen
  }
  return KDPoint(x,y);
}
