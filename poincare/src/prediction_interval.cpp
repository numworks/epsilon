#include <poincare/prediction_interval.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

PredictionInterval::PredictionInterval() :
  Function("prediction95")
{
}

bool PredictionInterval::isValidNumberOfArguments(int numberOfArguments) {
  return (numberOfArguments == 2);
}

Expression::Type PredictionInterval::type() const {
  return Type::PredictionInterval;
}

Expression * PredictionInterval::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  PredictionInterval * pi = new PredictionInterval();
  pi->setArgument(newOperands, numberOfOperands, cloneOperands);
  return pi;
}

float PredictionInterval::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  return NAN;
}

Expression * PredictionInterval::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float p = m_args[0]->approximate(context, angleUnit);
  float n = m_args[1]->approximate(context, angleUnit);
  Expression * operands[2];
  operands[0] = new Complex(Complex::Float(p - 1.96f*sqrtf(p*(1.0f-p))/sqrtf(n)));
  operands[1] = new Complex(Complex::Float(p + 1.96f*sqrtf(p*(1.0f-p))/sqrtf(n)));
  return new Matrix(new MatrixData(operands, 2, 2, 1, false));
}

}

