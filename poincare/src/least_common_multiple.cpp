#include <poincare/least_common_multiple.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

LeastCommonMultiple::LeastCommonMultiple() :
  Function("lcm")
{
}

Expression::Type LeastCommonMultiple::type() const {
  return Type::LeastCommonMultiple;
}

Expression * LeastCommonMultiple::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  LeastCommonMultiple * lcm = new LeastCommonMultiple();
  lcm->setArgument(newOperands, numberOfOperands, cloneOperands);
  return lcm;
}

float LeastCommonMultiple::privateApproximate(Context& context, AngleUnit angleUnit) const {
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
  int product = a*b;
  int r = 0;
  while((int)b!=0){
    r = a - ((int)(a/b))*b;
    a = b;
    b = r;
  }
  return roundf((float)(product/a));
}

}

