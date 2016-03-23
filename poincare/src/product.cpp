#include <poincare/product.h>
#include "layout/horizontal_layout.h"

Product::Product(Expression * first_factor, Expression * second_factor) {
  m_children[0] = first_factor;
  m_children[1] = second_factor;
}

Product::~Product() {
  delete m_children[1];
  delete m_children[0];
}

float Product::approximate(Context& context) {
  return m_children[0]->approximate(context) * m_children[1]->approximate(context);
}

expression_type_t Product::type() {
  return Product::Type;
}


ExpressionLayout * Product::createLayout(ExpressionLayout * parent) {
  return new HorizontalLayout(parent, m_children[0], '*', m_children[1]);
}
