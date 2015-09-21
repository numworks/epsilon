//#include <string.h>
#include "horizontal_layout.h"

static inline KDCoordinate max(KDCoordinate a, KDCoordinate b) {
  return (a > b ? a : b);
}

HorizontalLayout::HorizontalLayout(Expression * left_expression, char symbol, Expression * right_expression) {
  m_children[0] = left_expression->createLayout();
  m_children[1] = right_expression->createLayout();
  m_symbol = symbol;
  /* Perform the layout */
  m_frame.x = 0;
  m_frame.y = 0;
  m_frame.width = m_children[0]->m_frame.width + 12 + m_children[1]->m_frame.width; // FIXME: 12 is not actually 12
  m_frame.height = max(m_children[0]->m_frame.height, m_children[1]->m_frame.height); // FIXME: height of m_symbol
}

HorizontalLayout::~HorizontalLayout() {
  delete m_children[1];
  delete m_children[2];
}

void HorizontalLayout::draw() {
  m_children[0]->draw();
  m_children[1]->draw();
}
/*

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
*/
