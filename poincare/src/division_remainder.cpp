#include <poincare/division_remainder.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

DivisionRemainder::DivisionRemainder() :
  Function("rem")
{
}

bool DivisionRemainder::isValidNumberOfArguments(int numberOfArguments) {
  return (numberOfArguments == 2);
}

Expression::Type DivisionRemainder::type() const {
  return Type::DivisionRemainder;
}

Expression * DivisionRemainder::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  DivisionRemainder * dr = new DivisionRemainder();
  dr->setArgument(newOperands, numberOfOperands, cloneOperands);
  return dr;
}

float DivisionRemainder::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float f1 = m_args[0]->approximate(context, angleUnit);
  float f2 = m_args[1]->approximate(context, angleUnit);
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return NAN;
  }
  return roundf(f1-f2*floorf(f1/f2));
}

}

