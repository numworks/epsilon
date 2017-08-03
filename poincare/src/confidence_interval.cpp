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

template<typename T>
Evaluation<T> * ConfidenceInterval::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * fInput = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * nInput = m_args[1]->evaluate<T>(context, angleUnit);
  T f = fInput->toScalar();
  T n = nInput->toScalar();
  delete fInput;
  delete nInput;
  if (isnan(f) || isnan(n) || n != (int)n || n < 0 || f < 0 || f > 1) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Complex<T> operands[2];
  operands[0] = Complex<T>::Float(f - 1/std::sqrt(n));
  operands[1] = Complex<T>::Float(f + 1/std::sqrt(n));
  return new ComplexMatrix<T>(operands, 2, 1);
}

}

