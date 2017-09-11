#include <poincare/permute_coefficient.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <math.h>

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

template<typename T>
Evaluation<T> * PermuteCoefficient::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * nInput = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * kInput = m_args[1]->evaluate<T>(context, angleUnit);
  T n = nInput->toScalar();
  T k = kInput->toScalar();
  delete nInput;
  delete kInput;
  if (isnan(n) || isnan(k) || n != (int)n || k != (int)k || n < 0.0f || k < 0.0f) {

    return new Complex<T>(Complex<T>::Float(NAN));
  }
  if (k > n) {
    return new Complex<T>(Complex<T>::Float(0));
  }
  T result = 1;
  for (int i = (int)n-(int)k+1; i <= (int)n; i++) {
    result *= i;
  }
  return new Complex<T>(Complex<T>::Float(round(result)));
}

}

