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

char Product::operatorChar() {
  return '*';
}

#ifdef DEBUG
int Product::getPrintableVersion(char* txt) {
  txt[0] = '*';
  return 1;
}
#endif
