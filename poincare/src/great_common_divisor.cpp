#include <poincare/great_common_divisor.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type GreatCommonDivisor::type() const {
  return Type::GreatCommonDivisor;
}

Expression * GreatCommonDivisor::clone() const {
  GreatCommonDivisor * a = new GreatCommonDivisor(m_operands, true);
  return a;
}

template<typename T>
Evaluation<T> * GreatCommonDivisor::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * f1Input = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * f2Input = operand(1)->evaluate<T>(context, angleUnit);
  T f1 = f1Input->toScalar();
  T f2 = f2Input->toScalar();
  delete f1Input;
  delete f2Input;
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int a = (int)f2;
  int b = (int)f1;
  if (f1 > f2) {
    b = a;
    a = (int)f1;
  }
  int r = 0;
  while((int)b!=0){
    r = a - ((int)(a/b))*b;
    a = b;
    b = r;
  }
  return new Complex<T>(Complex<T>::Float(std::round((T)a)));
}

}

