#include <poincare/round.h>

extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

Round::Round() :
  Function("round", 2)
{
}

Expression::Type Round::type() const {
  return Type::Round;
}

Expression * Round::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Round * r = new Round();
  r->setArgument(newOperands, numberOfOperands, cloneOperands);
  return r;
}

Evaluation * Round::privateEvaluate(Context & context, AngleUnit angleUnit) const {
  Evaluation * f1Entry = m_args[0]->evaluate(context, angleUnit);
  Evaluation * f2Entry = m_args[1]->evaluate(context, angleUnit);
  float f1 = f1Entry->toFloat();
  float f2 = f2Entry->toFloat();
  delete f1Entry;
  delete f2Entry;
  if (isnan(f2) || f2 != (int)f2) {
    return new Complex(Complex::Float(NAN));
  }
  float err = powf(10.0f, (int)f2);
  return new Complex(Complex::Float(roundf(f1*err)/err));
}

}


