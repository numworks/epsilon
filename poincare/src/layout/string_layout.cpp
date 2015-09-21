#include "string_layout.h"

StringLayout::StringLayout(const char * string) {
  m_string = string;
  //FIXME: Copy the string?
}

void StringLayout::draw() {
  KDDrawString(m_string, (KDPoint){.x = 0, .y = 0});
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
