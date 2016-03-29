#include <string.h>
#include <assert.h>
#include "fraction_layout.h"

static inline KDCoordinate max(KDCoordinate a, KDCoordinate b) {
  return (a > b ? a : b);
}

#define FRACTION_BORDER_LENGTH 2
#define FRACTION_LINE_MARGIN 2
#define FRACTION_LINE_HEIGHT 1

FractionLayout::FractionLayout(ExpressionLayout * numerator_layout, ExpressionLayout * denominator_layout) :
ExpressionLayout(), m_numerator_layout(numerator_layout), m_denominator_layout(denominator_layout) {
  m_numerator_layout->setParent(this);
  m_denominator_layout->setParent(this);
  m_baseline = m_numerator_layout->size().height + FRACTION_LINE_MARGIN + KDStringSize(" ").height/2;
}

FractionLayout::~FractionLayout() {
  delete m_denominator_layout;
  delete m_numerator_layout;
}

void FractionLayout::render(KDPoint point) {
  KDCoordinate fractionLineY = point.y + m_numerator_layout->size().height + FRACTION_LINE_MARGIN;

  KDDrawLine(
      KDPointMake(point.x, fractionLineY),
      KDPointMake(point.x + size().width, fractionLineY),
      0xFF);
}

KDSize FractionLayout::computeSize() {
  KDSize s;
  s.width = max(m_numerator_layout->size().width, m_denominator_layout->size().width) + 2*FRACTION_BORDER_LENGTH;
  s.height = m_numerator_layout->size().height + FRACTION_LINE_MARGIN
    + FRACTION_LINE_HEIGHT + FRACTION_LINE_MARGIN + m_denominator_layout->size().height;
  return s;
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
  KDPoint p;
  if (child == m_numerator_layout) {
    p.x = (KDCoordinate)((size().width - m_numerator_layout->size().width)/2);
    p.y = 0;
  } else if (child == m_denominator_layout) {
    p.x = (KDCoordinate)((size().width - m_denominator_layout->size().width)/2);
    p.y = (KDCoordinate)(m_numerator_layout->size().height + 2*FRACTION_LINE_MARGIN + FRACTION_LINE_HEIGHT);
  } else {
    assert(false); // Should not happen
  }
  return p;
}
