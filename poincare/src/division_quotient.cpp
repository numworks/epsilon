#include <poincare/division_quotient.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

DivisionQuotient::DivisionQuotient() :
  Function("quo", 2)
{
}

Expression::Type DivisionQuotient::type() const {
  return Type::DivisionQuotient;
}

Expression * DivisionQuotient::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  DivisionQuotient * dq = new DivisionQuotient();
  dq->setArgument(newOperands, numberOfOperands, cloneOperands);
  return dq;
}

template<typename T>
Evaluation<T> * DivisionQuotient::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * f1Input = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * f2Input = m_args[1]->evaluate<T>(context, angleUnit);
  T f1 = f1Input->toScalar();
  T f2 = f2Input->toScalar();
  delete f1Input;
  delete f2Input;
  if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  return new Complex<T>(Complex<T>::Float(std::floor(f1/f2)));
}

}

