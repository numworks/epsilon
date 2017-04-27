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

float Product::approximateWithNextTerm(float sequence, float nextTerm) const {
  return sequence * nextTerm;
}

ExpressionLayout * Product::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const {
  return new ProductLayout(subscriptLayout, superscriptLayout, argumentLayout);
}

Expression * Product::evaluateWithNextTerm(Expression ** args, Context& context, AngleUnit angleUnit) const {
  Expression * multiplication = new Multiplication(args, true);
  Expression * result = multiplication->evaluate(context, angleUnit);
  delete multiplication;
  return result;
}

}
