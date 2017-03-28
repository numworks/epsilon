#include <poincare/permute_coefficient.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

PermuteCoefficient::PermuteCoefficient() :
  Function("permute")
{
}

bool PermuteCoefficient::hasValidNumberOfArguments() const {
  return (m_numberOfArguments == 2);
}

Expression::Type PermuteCoefficient::type() const {
  return Type::PermuteCoefficient;
}

Expression * PermuteCoefficient::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  PermuteCoefficient * pc = new PermuteCoefficient();
  pc->setArgument(newOperands, numberOfOperands, cloneOperands);
  return pc;
}

float PermuteCoefficient::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float n = m_args[0]->approximate(context, angleUnit);
  float k = m_args[1]->approximate(context, angleUnit);
  if (isnan(n) || isnan(k) || n != (int)n || k != (int)k) {
    return NAN;
  }
  float result = 1.0f;
  for (int i = (int)n-(int)k+1; i <= (int)n; i++) {
    result *= i;
  }
  return roundf(result);
}

}

