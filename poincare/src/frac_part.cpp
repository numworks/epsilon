#include <poincare/frac_part.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

FracPart::FracPart() :
  Function("frac")
{
}

Expression::Type FracPart::type() const {
  return Type::FracPart;
}

Expression * FracPart::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  FracPart * fp = new FracPart();
  fp->setArgument(newOperands, numberOfOperands, cloneOperands);
  return fp;
}

float FracPart::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float f = m_args[0]->approximate(context, angleUnit);
  return f-floorf(f);
}

}


