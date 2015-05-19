#include <poincare/fraction.h>

static inline KDCoordinate max(KDCoordinate a, KDCoordinate b) {
  return (a > b ? a : b);
}

#define FRACTION_BORDER_LENGTH 2
#define FRACTION_LINE_MARGIN 2
#define FRACTION_LINE_HEIGHT 1

Fraction::Fraction(Expression * numerator, Expression * denominator) :
  m_numerator(numerator),
  m_denominator(denominator) {
}

void Fraction::layout() {
  m_numerator->layout();
  m_denominator->layout();

  KDRect numFrame = m_numerator->m_frame;
  KDRect denFrame = m_denominator->m_frame;

  m_frame.width = max(numFrame.width, denFrame.width) + 2*FRACTION_BORDER_LENGTH;
  m_frame.height = numFrame.height + FRACTION_LINE_MARGIN + FRACTION_LINE_HEIGHT + FRACTION_LINE_MARGIN + denFrame.height;

  m_numerator->m_frame.origin = {
    .x = (KDCoordinate)((m_frame.width - numFrame.width)/2),
    .y = 0
  };

  m_denominator->m_frame.origin = {
    .x = (KDCoordinate)((m_frame.width - denFrame.width)/2),
    .y = (KDCoordinate)(numFrame.height + 2*FRACTION_LINE_MARGIN + FRACTION_LINE_HEIGHT)
  };
}

void Fraction::draw() {
  m_numerator->draw();
  m_denominator->draw();

  KDCoordinate fractionLineY = m_numerator->m_frame.height + FRACTION_LINE_MARGIN;

  KDDrawLine((KDPoint){.x = 0, .y = fractionLineY},
      (KDPoint){.x = m_frame.width, .y = fractionLineY});
}
