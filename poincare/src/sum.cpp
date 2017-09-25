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

int Sum::emptySequenceValue() const {
  return 0;
}

ExpressionLayout * Sum::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const {
  return new SumLayout(subscriptLayout, superscriptLayout, argumentLayout);
}

template<typename T>
Evaluation<T> * Sum::templatedEvaluateWithNextTerm(Evaluation<T> * a, Evaluation<T> * b) const {
  if (a->numberOfOperands() == 1 && b->numberOfOperands() == 1) {
    return new Complex<T>(Addition::compute(*(a->complexOperand(0)), *(b->complexOperand(0))));
  }
  if (a->numberOfOperands() == 1) {
    return Addition::computeOnComplexAndMatrix(a->complexOperand(0), b);
  }
  return Addition::computeOnMatrices(a, b);
}

}
