#include <poincare/fraction.h>
#include <string.h>

static inline KDCoordinate max(KDCoordinate a, KDCoordinate b) {
  return (a > b ? a : b);
}

#define NUMERATOR m_children[0]
#define DENOMINATOR m_children[1]

#define FRACTION_BORDER_LENGTH 2
#define FRACTION_LINE_MARGIN 2
#define FRACTION_LINE_HEIGHT 1

Addition::Addition(Expression * first_operand, Expression * second_operand) {
  m_children[0] = first_operand;
  m_children[1] = second_operand;
}

Expression ** Fraction::children() {
  return m_children;
}

int Addition::numberOfChildren() {
  return 2;
}

float Addition::approximate() {
  return m_children[0]->approximate() + m_children[1]->approximate();
}

ExpressionLayout * Addition::layout() {
  return HorizontalLayout(m_children[0], "+", m_children[1]);
}
