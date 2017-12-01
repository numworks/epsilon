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

ExpressionLayout * Sum::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const {
  return new SumLayout(subscriptLayout, superscriptLayout, argumentLayout);
}

template<typename T>
Expression * Sum::templatedApproximateWithNextTerm(Expression * a, Expression * b) const {
  if (a->type() == Type::Complex && b->type() == Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(a);
    Complex<T> * d = static_cast<Complex<T> *>(b);
    return new Complex<T>(Addition::compute(*c, *d));
  }
  if (a->type() == Type::Complex) {
    Complex<T> * c = static_cast<Complex<T> *>(a);
    assert(b->type() == Type::Matrix);
    Matrix * m = static_cast<Matrix *>(b);
    return Addition::computeOnComplexAndMatrix(c, m);
  }
  assert(a->type() == Type::Matrix);
  assert(b->type() == Type::Matrix);
  Matrix * m = static_cast<Matrix *>(a);
  Matrix * n = static_cast<Matrix *>(b);
  return Addition::computeOnMatrices<T>(m, n);
}

}
