#include <poincare/great_common_divisor.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

GreatCommonDivisor::GreatCommonDivisor() :
  Function("gcd", 2)
{
}

Expression::Type GreatCommonDivisor::type() const {
  return Type::GreatCommonDivisor;
}

Expression * GreatCommonDivisor::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  GreatCommonDivisor * gcd = new GreatCommonDivisor();
  gcd->setArgument(newOperands, numberOfOperands, cloneOperands);
  return gcd;
}

Evaluation * GreatCommonDivisor::privateEvaluate(Context & context, AngleUnit angleUnit) const {
  Evaluation * f1Input = m_args[0]->evaluate(context, angleUnit);
  Evaluation * f2Input = m_args[1]->evaluate(context, angleUnit);
  float f1 = f1Input->toFloat();
  float f2 = f2Input->toFloat();
  delete f1Input;
  delete f2Input;
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex(Complex::Float(NAN));
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
  return new Complex(Complex::Float(roundf((float)a)));

}

}

