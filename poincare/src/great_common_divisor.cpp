#include <poincare/great_common_divisor.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

GreatCommonDivisor::GreatCommonDivisor() :
  Function("gcd")
{
}

Expression::Type GreatCommonDivisor::type() const {
  return Type::GreatCommonDivisor;
}

Expression * GreatCommonDivisor::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  GreatCommonDivisor * gcd = new GreatCommonDivisor();
  gcd->setArgument(newOperands, numberOfOperands, cloneOperands);
  return gcd;
}

float GreatCommonDivisor::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float f1 = m_args[0]->approximate(context, angleUnit);
  float f2 = m_args[1]->approximate(context, angleUnit);
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return NAN;
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
  return roundf((float)a);
}

}

