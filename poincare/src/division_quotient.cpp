#include <poincare/division_quotient.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

DivisionQuotient::DivisionQuotient() :
  Function("quo", 2)
{
}

Expression::Type DivisionQuotient::type() const {
  return Type::DivisionQuotient;
}

Expression * DivisionQuotient::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  DivisionQuotient * dq = new DivisionQuotient();
  dq->setArgument(newOperands, numberOfOperands, cloneOperands);
  return dq;
}

Evaluation * DivisionQuotient::privateEvaluate(Context & context, AngleUnit angleUnit) const {
  Evaluation * f1Input = m_args[0]->evaluate(context, angleUnit);
  Evaluation * f2Input = m_args[1]->evaluate(context, angleUnit);
  float f1 = f1Input->toFloat();
  float f2 = f2Input->toFloat();
  delete f1Input;
  delete f2Input;
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex(Complex::Float(NAN));
  }
  return new Complex(Complex::Float(floorf(f1/f2)));
}

}

