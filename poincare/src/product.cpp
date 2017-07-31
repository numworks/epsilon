#include <poincare/product.h>
#include <poincare/multiplication.h>
#include "layout/product_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
}

namespace Poincare {

Product::Product() :
  Sequence("product")
{
}

Expression::Type Product::type() const {
  return Type::Product;
}

Expression * Product::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Product * p = new Product();
  p->setArgument(newOperands, numberOfOperands, cloneOperands);
  return p;
}

float Product::emptySequenceValue() const {
  return 1.0f;
}

ExpressionLayout * Product::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const {
  return new ProductLayout(subscriptLayout, superscriptLayout, argumentLayout);
}

Evaluation * Product::evaluateWithNextTerm(Evaluation * a, Evaluation * b) const {
  if (a->numberOfOperands() == 1 && b->numberOfOperands() == 1) {
    return new Complex(Multiplication::compute(*(a->complexOperand(0)), *(b->complexOperand(0))));
  }
  if (a->numberOfOperands() == 1) {
    return Multiplication::computeOnComplexAndMatrix(a->complexOperand(0), b);
  }
  return Multiplication::computeOnMatrices(a, b);
}

}
