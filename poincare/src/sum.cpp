#include <poincare/sum.h>
#include <poincare/addition.h>
#include "layout/sum_layout.h"
extern "C" {
#include <assert.h>
#include <math.h>
#include <stdlib.h>
}

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

float Sum::approximateWithNextTerm(float sequence, float nextTerm) const {
  return sequence + nextTerm;
}

ExpressionLayout * Sum::createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const {
  return new SumLayout(subscriptLayout, superscriptLayout, argumentLayout);
}

Expression * Sum::evaluateWithNextTerm(Expression ** args, Context& context, AngleUnit angleUnit) const {
  Expression * addition = new Addition(args, true);
  Expression * result = addition->evaluate(context, angleUnit);
  delete addition;
  return result;
}

}
