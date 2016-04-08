extern "C" {
#include <assert.h>
}

#include <poincare/product.h>

Expression * Product::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) {
  return new Product(newOperands, numberOfOperands, cloneOperands);
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
