#include <poincare/confidence_interval.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

ConfidenceInterval::ConfidenceInterval() :
  Function("confidence")
{
}

Expression::Type ConfidenceInterval::type() const {
  return Type::ConfidenceInterval;
}

Expression * ConfidenceInterval::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  ConfidenceInterval * ci = new ConfidenceInterval();
  ci->setArgument(newOperands, numberOfOperands, cloneOperands);
  return ci;
}

float ConfidenceInterval::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return NAN;
}

Expression * ConfidenceInterval::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float f = m_args[0]->approximate(context, angleUnit);
  float n = m_args[1]->approximate(context, angleUnit);
  Expression * operands[2];
  operands[0] = new Complex(Complex::Float(f - 1.0f/sqrtf(n)));
  operands[1] = new Complex(Complex::Float(f + 1.0f/sqrtf(n)));
  return new Matrix(new MatrixData(operands, 2, 2, 1, false));
}

}

