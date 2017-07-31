#include <poincare/permute_coefficient.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

PermuteCoefficient::PermuteCoefficient() :
  Function("permute", 2)
{
}

Expression::Type PermuteCoefficient::type() const {
  return Type::PermuteCoefficient;
}

Expression * PermuteCoefficient::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  PermuteCoefficient * pc = new PermuteCoefficient();
  pc->setArgument(newOperands, numberOfOperands, cloneOperands);
  return pc;
}

Evaluation * PermuteCoefficient::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation * nInput = m_args[0]->evaluate(context, angleUnit);
  Evaluation * kInput = m_args[1]->evaluate(context, angleUnit);
  float n = nInput->toFloat();
  float k = kInput->toFloat();
  delete nInput;
  delete kInput;
  if (isnan(n) || isnan(k) || n != (int)n || k != (int)k || n < 0.0f || k < 0.0f) {

    return new Complex(Complex::Float(NAN));
  }
  if (k > n) {
    return new Complex(Complex::Float(0.0f));
  }
  float result = 1.0f;
  for (int i = (int)n-(int)k+1; i <= (int)n; i++) {
    result *= i;
  }
  return new Complex(Complex::Float(roundf(result)));
}

}

