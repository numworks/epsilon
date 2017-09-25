#include <poincare/division_quotient.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type DivisionQuotient::type() const {
  return Type::DivisionQuotient;
}

Expression * DivisionQuotient::clone() const {
  DivisionQuotient * a = new DivisionQuotient(m_operands, true);
  return a;
}

template<typename T>
Evaluation<T> * DivisionQuotient::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * f1Input = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * f2Input = operand(1)->evaluate<T>(context, angleUnit);
  T f1 = f1Input->toScalar();
  T f2 = f2Input->toScalar();
  delete f1Input;
  delete f2Input;
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  return new Complex<T>(Complex<T>::Float(std::floor(f1/f2)));
}

}

