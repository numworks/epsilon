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

ExpressionLayout * Product::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * argumentLayout, ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout) const {
  return new ProductLayout(argumentLayout, subscriptLayout, superscriptLayout, false);
}

template<typename T>
Evaluation<T> * Product::templatedApproximateWithNextTerm(Evaluation<T> * a, Evaluation<T> * b) const {
  if (a->type() == Evaluation<T>::Type::Complex && b->type() == Evaluation<T>::Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(a);
    Complex<T> * d = static_cast<Complex<T> *>(b);
    return new Complex<T>((*c)*(*d));
  }
  if (a->type() == Evaluation<T>::Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(a);
    assert(b->type() == Evaluation<T>::Type::MatrixComplex);
    MatrixComplex<T> * m = static_cast<MatrixComplex<T> *>(b);
    return new MatrixComplex<T>(Multiplication::computeOnComplexAndMatrix(*c, *m));
  }
  assert(a->type() == Evaluation<T>::Type::MatrixComplex);
  assert(b->type() == Evaluation<T>::Type::MatrixComplex);
  MatrixComplex<T> * m = static_cast<MatrixComplex<T> *>(a);
  MatrixComplex<T> * n = static_cast<MatrixComplex<T> *>(b);
  return new MatrixComplex<T>(Multiplication::computeOnMatrices<T>(*m, *n));
}

}
