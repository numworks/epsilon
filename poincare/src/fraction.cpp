#include <poincare/fraction.h>
#include <string.h>
#include "layout/horizontal_layout.h"

/*
static inline KDCoordinate max(KDCoordinate a, KDCoordinate b) {
  return (a > b ? a : b);
}
*/

#define NUMERATOR m_children[0]
#define DENOMINATOR m_children[1]

#define FRACTION_BORDER_LENGTH 2
#define FRACTION_LINE_MARGIN 2
#define FRACTION_LINE_HEIGHT 1

Fraction::Fraction(Expression * numerator, Expression * denominator) {
  m_children[0] = numerator;
  m_children[1] = denominator;
  m_children[2] = NULL;
}
/*

Expression ** Fraction::children() {
  return m_children;
}
*/

ExpressionLayout * Fraction::createLayout(ExpressionLayout * parent) {
  return new HorizontalLayout(parent, NUMERATOR, '-', DENOMINATOR);
}
/*
void Fraction::layout() {
  KDRect numFrame = NUMERATOR->m_frame;
  KDRect denFrame = DENOMINATOR->m_frame;

  m_frame.width = max(numFrame.width, denFrame.width) + 2*FRACTION_BORDER_LENGTH;
  m_frame.height = numFrame.height + FRACTION_LINE_MARGIN + FRACTION_LINE_HEIGHT + FRACTION_LINE_MARGIN + denFrame.height;

  NUMERATOR->m_frame.origin = {
    .x = (KDCoordinate)((m_frame.width - numFrame.width)/2),
    .y = 0
  };

  DENOMINATOR->m_frame.origin = {
    .x = (KDCoordinate)((m_frame.width - denFrame.width)/2),
    .y = (KDCoordinate)(numFrame.height + 2*FRACTION_LINE_MARGIN + FRACTION_LINE_HEIGHT)
  };
}

void Fraction::draw() {
  KDCoordinate fractionLineY = NUMERATOR->m_frame.height + FRACTION_LINE_MARGIN;

  KDDrawLine((KDPoint){.x = 0, .y = fractionLineY},
      (KDPoint){.x = m_frame.width, .y = fractionLineY});
}
*/

float Fraction::approximate() {
  // TODO: handle division by zero
  return m_children[0]->approximate()/m_children[1]->approximate();
}
