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
Complex<T> * Sum::templatedEvaluateWithNextTerm(Complex<T> * a, Complex<T> * b) const {
  return new Complex<T>(Addition::compute(*a, *b));
}

}
