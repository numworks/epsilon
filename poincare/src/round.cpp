#include <poincare/round.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

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

template<typename T>
Evaluation<T> * Round::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * f1Entry = m_args[0]->evaluate<T>(context, angleUnit);
  Evaluation<T> * f2Entry = m_args[1]->evaluate<T>(context, angleUnit);
  T f1 = f1Entry->toScalar();
  T f2 = f2Entry->toScalar();
  delete f1Entry;
  delete f2Entry;
  if (std::isnan(f2) || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T err = std::pow(10, std::floor(f2));
  return new Complex<T>(Complex<T>::Float(std::round(f1*err)/err));
}

}


