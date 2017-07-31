#include <poincare/least_common_multiple.h>
#include <poincare/complex.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

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

Evaluation * LeastCommonMultiple::privateEvaluate(Context & context, AngleUnit angleUnit) const {
  Evaluation * f1Input = m_args[0]->evaluate(context, angleUnit);
  Evaluation * f2Input = m_args[1]->evaluate(context, angleUnit);
  float f1 = f1Input->toFloat();
  float f2 = f2Input->toFloat();
  delete f1Input;
  delete f2Input;
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2 || f1 == 0.0f || f2 == 0.0f) {
    return new Complex(Complex::Float(NAN));
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
  return new Complex(Complex::Float(roundf((float)(product/a))));
}

}

