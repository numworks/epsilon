#include <poincare/confidence_interval.h>
#include <poincare/matrix.h>
#include <poincare/complex_matrix.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ConfidenceInterval::type() const {
  return Type::ConfidenceInterval;
}

Expression * ConfidenceInterval::clone() const {
  ConfidenceInterval * a = new ConfidenceInterval(m_operands, true);
  return a;
}

template<typename T>
Evaluation<T> * ConfidenceInterval::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * fInput = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * nInput = operand(1)->evaluate<T>(context, angleUnit);
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

