#include <poincare/product.h>
#include <poincare/multiplication.h>
#include "layout/product_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Product::type() const {
  return Type::Product;
}

Expression * Product::clone() const {
  Product * a = new Product(m_operands, true);
  return a;
}

const char * Product::name() const {
  return "product";
}

int Product::emptySequenceValue() const {
  return 1;
}

ExpressionLayout * Product::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const {
  return new ProductLayout(subscriptLayout, superscriptLayout, argumentLayout);
}

template<typename T>
Complex<T> * Product::templatedEvaluateWithNextTerm(Complex<T> * a, Complex<T> * b) const {
  return new Complex<T>(Multiplication::compute(*a, *b));
}

}
