#include <poincare/fraction.h>
#include <string.h>
#include <assert.h>
#include "fraction_layout.h"

static inline KDCoordinate max(KDCoordinate a, KDCoordinate b) {
  return (a > b ? a : b);
}

#define FRACTION_BORDER_LENGTH 2
#define FRACTION_LINE_MARGIN 2
#define FRACTION_LINE_HEIGHT 1

FractionLayout::FractionLayout(ExpressionLayout * parent, Expression * numerator, Expression * denominator) :
ExpressionLayout(parent) {
  m_numerator = numerator->createLayout(this);
  m_denominator = denominator->createLayout(this);
}

FractionLayout::~FractionLayout() {
  delete m_denominator;
  delete m_numerator;
}

void FractionLayout::render(KDPoint origin) {
  KDCoordinate fractionLineY = m_numerator->size().height + FRACTION_LINE_MARGIN;

  KDDrawLine((KDPoint){.x = 0, .y = fractionLineY},
      (KDPoint){.x = size().width, .y = fractionLineY});
}

KDSize FractionLayout::computeSize() {
  KDSize s;
  s.width = max(m_numerator->size().width, m_denominator->size().width) + 2*FRACTION_BORDER_LENGTH;
  s.height = m_numerator->size().height + FRACTION_LINE_MARGIN + FRACTION_LINE_HEIGHT + FRACTION_LINE_MARGIN + m_denominator->size().height;
  return s;
}

ExpressionLayout * FractionLayout::child(uint16_t index) {
  switch (index) {
    case 0:
      return m_numerator;
    case 1:
      return m_denominator;
    default:
      return nullptr;
  }
}

KDPoint FractionLayout::positionOfChild(ExpressionLayout * child) {
  KDPoint p;
  if (child == m_numerator) {
    p.x = (KDCoordinate)((size().width - m_numerator->size().width)/2);
    p.y = 0;
  } else if (child == m_denominator) {
    p.x = (KDCoordinate)((size().width - m_denominator->size().width)/2);
    p.y = (KDCoordinate)(size().height + 2*FRACTION_LINE_MARGIN + FRACTION_LINE_HEIGHT);
  } else {
    assert(false); // Should not happen
  }
  return p;
}
