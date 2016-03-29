#include <string.h>
#include <assert.h>
#include "exponent_layout.h"

// TODO(fraimundo): Find a better name.
#define EXPONENT_HEIGHT 5

ExponentLayout::ExponentLayout(ExpressionLayout * base_layout, ExpressionLayout * exponent_layout) :
ExpressionLayout(), m_base_layout(base_layout), m_exponent_layout(exponent_layout) {
  m_base_layout->setParent(this);
  m_exponent_layout->setParent(this);
  m_baseline = m_exponent_layout->baseline() + m_base_layout->baseline() - EXPONENT_HEIGHT;
}

ExponentLayout::~ExponentLayout() {
  delete m_exponent_layout;
  delete m_base_layout;
}

// There is nothing to draw for a power, only the position of the children matters
void ExponentLayout::render(KDPoint point) { }

KDSize ExponentLayout::computeSize() {
  KDSize s;
  KDSize exponent_size, base_size;
  exponent_size = m_exponent_layout->size();
  base_size = m_base_layout->size();
  s.height = base_size.height + exponent_size.height - EXPONENT_HEIGHT;
  s.width = base_size.width + exponent_size.width;
  return s;
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
  KDPoint p;
  if (child == m_base_layout) {
    p.x = 0;
    p.y = m_exponent_layout->baseline() - EXPONENT_HEIGHT;
  } else if (child == m_exponent_layout) {
    p.x = m_base_layout->size().width;
    p.y = 0;
  } else {
    assert(false); // Should not happen
  }
  return p;
}
