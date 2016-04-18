extern "C" {
#include <assert.h>
}

#include <poincare/product.h>

Expression * Product::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  return new Product(newOperands, numberOfOperands, cloneOperands);
}

float Product::operateApproximatevelyOn(float a, float b) const {
  return a*b;
}

Expression::Type Product::type() const {
  return Expression::Type::Product;
}

char Product::operatorChar() const {
  return '*';
}
