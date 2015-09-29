#include <poincare/addition.h>
#include "layout/horizontal_layout.h"

Addition::Addition(Expression * first_operand, Expression * second_operand) {
  m_children[0] = first_operand;
  m_children[1] = second_operand;
}

Addition::~Addition() {
  delete m_children[1];
  delete m_children[0];
}

float Addition::approximate() {
  return m_children[0]->approximate() + m_children[1]->approximate();
}

ExpressionLayout * Addition::createLayout(ExpressionLayout * parent) {
  return new HorizontalLayout(parent, m_children[0], '+', m_children[1]);
}
