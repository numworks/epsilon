#include <poincare/product.h>
#include "layout/horizontal_layout.h"
extern "C" {
#include <stdlib.h>
}

Product::Product(Expression * first_factor, Expression * second_factor) {
  m_numberOfFactors = 2;
  m_factors = (Expression **)malloc(2*sizeof(Expression *));
  m_factors[0] = first_factor;
  m_factors[1] = second_factor;
}

Product::~Product() {
  for (int i=0; i<m_numberOfFactors; i++) {
    delete m_factors[i];
  }
}

float Product::approximate(Context& context) {
  float result = m_factors[0]->approximate(context);
  for (size_t i=1; i<m_numberOfFactors; i++) {
    result *= m_factors[i]->approximate(context);
  }
  return result;
}

int Product::numberOfFactors() {
  return m_numberOfFactors;
}

Expression * Product::factor(int i) {
  return m_factors[i];
}

expression_type_t Product::type() {
  return Product::Type;
}

ExpressionLayout * Product::createLayout(ExpressionLayout * parent) {
  //FIXME: There can be more than two factors now! :-)
  return new HorizontalLayout(parent, m_factors[0], '*', m_factors[1]);
}
