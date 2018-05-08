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
Expression * Product::templatedApproximateWithNextTerm(Expression * a, Expression * b) const {
  if (a->type() == Type::Complex && b->type() == Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(a);
    Complex<T> * d = static_cast<Complex<T> *>(b);
    return new Complex<T>(Multiplication::compute(*c, *d));
  }
  if (a->type() == Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(a);
    assert(b->type() == Type::Matrix);
    Matrix * m = static_cast<Matrix *>(b);
    return Multiplication::computeOnComplexAndMatrix(c, m);
  }
  assert(a->type() == Type::Matrix);
  assert(b->type() == Type::Matrix);
  Matrix * m = static_cast<Matrix *>(a);
  Matrix * n = static_cast<Matrix *>(b);
  return Multiplication::computeOnMatrices<T>(m, n);
}

}
