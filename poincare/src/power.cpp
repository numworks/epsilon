#include <poincare/power.h>
#include <string.h>

#define BASE m_children[0]
#define EXPONENT m_children[1]

Power::Power(Expression * base, Expression * exponent) {
  m_children[0] = base;
  m_children[1] = exponent;
  m_children[2] = NULL;
}

Expression ** Power::children() {
  return m_children;
}

void Power::layout() {
  m_frame.width = BASE->m_frame.width + EXPONENT->m_frame.width;
  m_frame.height = BASE->m_frame.height + EXPONENT->m_frame.height;

  BASE->m_frame.origin = {
    .x = 0,
    .y = EXPONENT->m_frame.height
  };

  EXPONENT->m_frame.origin = {
    .x = BASE->m_frame.width,
    .y = 0
  };
}
