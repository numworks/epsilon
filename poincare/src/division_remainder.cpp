#include <poincare/division_remainder.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

DivisionRemainder::DivisionRemainder() :
  Function("rem", 2)
{
}

Expression::Type DivisionRemainder::type() const {
  return Type::DivisionRemainder;
}

Expression * DivisionRemainder::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  DivisionRemainder * dr = new DivisionRemainder();
  dr->setArgument(newOperands, numberOfOperands, cloneOperands);
  return dr;
}

template<typename T>
Evaluation<T> * DivisionRemainder::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * f1Input = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * f2Input = m_args[1]->evaluate<T>(context, angleUnit);
  T f1 = f1Input->toScalar();
  T f2 = f2Input->toScalar();
  delete f1Input;
  delete f2Input;
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  return new Complex<T>(Complex<T>::Float(std::round(f1-f2*std::floor(f1/f2))));
}

}

