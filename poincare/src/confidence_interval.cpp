#include <poincare/confidence_interval.h>
#include <poincare/matrix.h>
#include <poincare/evaluation.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

ConfidenceInterval::ConfidenceInterval() :
  Function("confidence", 2)
{
}

Expression::Type ConfidenceInterval::type() const {
  return Type::ConfidenceInterval;
}

Expression * ConfidenceInterval::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ConfidenceInterval * ci = new ConfidenceInterval();
  ci->setArgument(newOperands, numberOfOperands, cloneOperands);
  return ci;
}

Evaluation * ConfidenceInterval::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * fInput = m_args[0]->evaluate(context, angleUnit);
  Evaluation * nInput = m_args[1]->evaluate(context, angleUnit);
  float f = fInput->toFloat();
  float n = nInput->toFloat();
  delete fInput;
  delete nInput;
  if (isnan(f) || isnan(n) || n != (int)n || n < 0.0f || f < 0.0f || f > 1.0f) {
    return new Complex(Complex::Float(NAN));
  }
  Complex operands[2];
  operands[0] = Complex::Float(f - 1.0f/std::sqrt(n));
  operands[1] = Complex::Float(f + 1.0f/std::sqrt(n));
  return new ComplexMatrix(operands, 2, 1);
}

}

