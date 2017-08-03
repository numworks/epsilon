#include <poincare/least_common_multiple.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

LeastCommonMultiple::LeastCommonMultiple() :
  Function("lcm", 2)
{
}

Expression::Type LeastCommonMultiple::type() const {
  return Type::LeastCommonMultiple;
}

Expression * LeastCommonMultiple::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  LeastCommonMultiple * lcm = new LeastCommonMultiple();
  lcm->setArgument(newOperands, numberOfOperands, cloneOperands);
  return lcm;
}

template<typename T>
Evaluation<T> * LeastCommonMultiple::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * f1Input = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * f2Input = m_args[1]->evaluate<T>(context, angleUnit);
  T f1 = f1Input->toScalar();
  T f2 = f2Input->toScalar();
  delete f1Input;
  delete f2Input;
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2 || f1 == 0.0f || f2 == 0.0f) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int a = (int)f2;
  int b = (int)f1;
  if (f1 > f2) {
    b = a;
    a = (int)f1;
  }
  int product = a*b;
  int r = 0;
  while((int)b!=0){
    r = a - ((int)(a/b))*b;
    a = b;
    b = r;
  }
  return new Complex<T>(Complex<T>::Float(std::round((T)(product/a))));
}

}

