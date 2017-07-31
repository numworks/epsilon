#include <poincare/prediction_interval.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

PredictionInterval::PredictionInterval() :
  Function("prediction95", 2)
{
}

Expression::Type PredictionInterval::type() const {
  return Type::PredictionInterval;
}

Expression * PredictionInterval::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  PredictionInterval * pi = new PredictionInterval();
  pi->setArgument(newOperands, numberOfOperands, cloneOperands);
  return pi;
}

Evaluation * PredictionInterval::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * pInput = m_args[0]->evaluate(context, angleUnit);
  Evaluation * nInput = m_args[1]->evaluate(context, angleUnit);
  float p = pInput->toFloat();
  float n = nInput->toFloat();
  delete pInput;
  delete nInput;
  if (isnan(p) || isnan(n) || n != (int)n || n < 0.0f || p < 0.0f || p > 1.0f) {
    return new Complex(Complex::Float(NAN));
  }
  Complex operands[2];
  operands[0] = Complex::Float(p - 1.96f*sqrtf(p*(1.0f-p))/sqrtf(n));
  operands[1] = Complex::Float(p + 1.96f*sqrtf(p*(1.0f-p))/sqrtf(n));
  return new ComplexMatrix(operands, 2, 1);
}

}

