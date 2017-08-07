#include <poincare/sum.h>
#include <poincare/addition.h>
#include "layout/sum_layout.h"
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

Sum::Sum() :
  Sequence("sum")
{
}

Expression::Type Sum::type() const {
  return Type::Sum;
}

Expression * Sum::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Sum * s = new Sum();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

float Sum::emptySequenceValue() const {
  return 0.0f;
}

ExpressionLayout * Sum::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const {
  return new SumLayout(subscriptLayout, superscriptLayout, argumentLayout);
}

Evaluation * Sum::evaluateWithNextTerm(Evaluation * a, Evaluation * b) const {
  if (a->numberOfOperands() == 1 && b->numberOfOperands() == 1) {
    return new Complex(Addition::compute(*(a->complexOperand(0)), *(b->complexOperand(0))));
  }
  if (a->numberOfOperands() == 1) {
    return Addition::computeOnComplexAndMatrix(a->complexOperand(0), b);
  }
  return Addition::computeOnMatrices(a, b);
}

}
