#include <poincare/sum.h>
#include <poincare/addition.h>
#include "layout/sum_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Sum::type() const {
  return Type::Sum;
}

Expression * Sum::clone() const {
  Sum * a = new Sum(m_operands, true);
  return a;
}

const char * Sum::name() const {
  return "sum";
}

int Sum::emptySequenceValue() const {
  return 0;
}

ExpressionLayout * Sum::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * argumentLayout, ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout) const {
  return new SumLayout(argumentLayout, subscriptLayout, superscriptLayout, false);
}

template<typename T>
Evaluation<T> * Sum::templatedApproximateWithNextTerm(Evaluation<T> * a, Evaluation<T> * b) const {
  if (a->type() == Evaluation<T>::Type::Complex && b->type() == Evaluation<T>::Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(a);
    Complex<T> * d = static_cast<Complex<T> *>(b);
    return new Complex<T>((*c)+(*d));
  }
  if (a->type() == Evaluation<T>::Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(a);
    assert(b->type() == Evaluation<T>::Type::MatrixComplex);
    MatrixComplex<T> * m = static_cast<MatrixComplex<T> *>(b);
    return new MatrixComplex<T>(Addition::computeOnComplexAndMatrix(*c, *m));
  }
  assert(a->type() == Evaluation<T>::Type::MatrixComplex);
  assert(b->type() == Evaluation<T>::Type::MatrixComplex);
  MatrixComplex<T> * m = static_cast<MatrixComplex<T> *>(a);
  MatrixComplex<T> * n = static_cast<MatrixComplex<T> *>(b);
  return new MatrixComplex<T>(Addition::computeOnMatrices<T>(*m, *n));
}

}
