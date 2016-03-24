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
  //FIXME: There can be more than two factors now! :-)
  return new HorizontalLayout(parent, operand(0), '*', operand(1));
}
