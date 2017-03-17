#include <poincare/round.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Round::Round() :
  Function("round")
{
}

bool Round::isValidNumberOfArguments(int numberOfArguments) {
  return (numberOfArguments == 2);
}

Expression::Type Round::type() const {
  return Type::Round;
}

Expression * Round::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 1);
  assert(newOperands != nullptr);
  Round * r = new Round();
  r->setArgument(newOperands, numberOfOperands, cloneOperands);
  return r;
}

float Round::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float f1 = m_args[0]->approximate(context, angleUnit);
  float f2 = m_args[1]->approximate(context, angleUnit);
  if (isnan(f2) || f2 != (int)f2) {
    return NAN;
  }
  float err = powf(10.0f, (int)f2);
  return roundf(f1*err)/err;
}

}


