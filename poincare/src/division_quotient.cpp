#include <poincare/division_quotient.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

DivisionQuotient::DivisionQuotient() :
  Function("quo")
{
}

bool DivisionQuotient::isValidNumberOfArguments(int numberOfArguments) {
  return (numberOfArguments == 2);
}

Expression::Type DivisionQuotient::type() const {
  return Type::DivisionQuotient;
}

Expression * DivisionQuotient::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  assert(newOperands != nullptr);
  DivisionQuotient * dq = new DivisionQuotient();
  dq->setArgument(newOperands, numberOfOperands, cloneOperands);
  return dq;
}

float DivisionQuotient::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  float f1 = m_args[0]->approximate(context, angleUnit);
  float f2 = m_args[1]->approximate(context, angleUnit);
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return NAN;
  }
  return floorf(f1/f2);
}

}

