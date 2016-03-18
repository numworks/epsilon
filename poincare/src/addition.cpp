#include <poincare/addition.h>
#include "layout/horizontal_layout.h"

Addition::Addition(Expression * first_operand, Expression * second_operand) {
  m_left = first_operand;
  m_right = second_operand;
}

Addition::~Addition() {
  delete m_left;
  delete m_right;
}

float Addition::approximate(Context& context) {
  return m_left->approximate(context) + m_right->approximate(context);
}

ExpressionLayout * Addition::createLayout(ExpressionLayout * parent) {
  return new HorizontalLayout(parent, m_left, '+', m_right);
}
