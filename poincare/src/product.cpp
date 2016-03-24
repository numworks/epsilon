#include <poincare/product.h>
#include "layout/horizontal_layout.h"
extern "C" {
#include <stdlib.h>
}

Expression * Product::clone() {
  return new Product(m_operands, m_numberOfOperands, true);
}

float Product::operateApproximatevelyOn(float a, float b) {
  return a*b;
}

Expression::Type Product::type() {
  return Expression::Type::Product;
}

ExpressionLayout * Product::createLayout(ExpressionLayout * parent) {
  return new HorizontalLayout(parent, m_operands, m_numberOfOperands, '*');
}
