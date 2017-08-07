#include <poincare/division_remainder.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

DivisionRemainder::DivisionRemainder() :
  Function("rem", 2)
{
}

Expression::Type DivisionRemainder::type() const {
  return Type::DivisionRemainder;
}

Expression * DivisionRemainder::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  DivisionRemainder * dr = new DivisionRemainder();
  dr->setArgument(newOperands, numberOfOperands, cloneOperands);
  return dr;
}

Evaluation * DivisionRemainder::privateEvaluate(Context & context, AngleUnit angleUnit) const {
  Evaluation * f1Input = m_args[0]->evaluate(context, angleUnit);
  Evaluation * f2Input = m_args[1]->evaluate(context, angleUnit);
  float f1 = f1Input->toFloat();
  float f2 = f2Input->toFloat();
  delete f1Input;
  delete f2Input;
  if (isnan(f1) || isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
    return new Complex(Complex::Float(NAN));
  }
  return new Complex(Complex::Float(std::round(f1-f2*std::floor(f1/f2))));
}

}

