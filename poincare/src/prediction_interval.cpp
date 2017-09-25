#include <poincare/prediction_interval.h>
#include <poincare/matrix.h>
#include <poincare/complex_matrix.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type PredictionInterval::type() const {
  return Type::PredictionInterval;
}

Expression * PredictionInterval::clone() const {
  PredictionInterval * a = new PredictionInterval(m_operands, true);
  return a;
}

template<typename T>
Evaluation<T> * PredictionInterval::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * pInput = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * nInput = operand(1)->evaluate<T>(context, angleUnit);
  T p = pInput->toScalar();
  T n = nInput->toScalar();
  delete pInput;
  delete nInput;
  if (isnan(p) || isnan(n) || n != (int)n || n < 0 || p < 0 || p > 1) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  Complex<T> operands[2];
  operands[0] = Complex<T>::Float(p - 1.96*std::sqrt(p*(1.0-p))/std::sqrt(n));
  operands[1] = Complex<T>::Float(p + 1.96*std::sqrt(p*(1.0-p))/std::sqrt(n));
  return new ComplexMatrix<T>(operands, 2, 1);
}

}

