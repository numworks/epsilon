#include <poincare/prediction_interval.h>
#include <poincare/matrix.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

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

template<typename T>
Evaluation<T> * PredictionInterval::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * pInput = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * nInput = m_args[1]->evaluate<T>(context, angleUnit);
  T p = pInput->toScalar();
  T n = nInput->toScalar();
  delete pInput;
  delete nInput;
  if (std::isnan(p) || std::isnan(n) || n != (int)n || n < 0 || p < 0 || p > 1) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Complex<T> operands[2];
  operands[0] = Complex<T>::Float(p - 1.96*std::sqrt(p*(1.0-p))/std::sqrt(n));
  operands[1] = Complex<T>::Float(p + 1.96*std::sqrt(p*(1.0-p))/std::sqrt(n));
  return new ComplexMatrix<T>(operands, 1, 2);
}

}

