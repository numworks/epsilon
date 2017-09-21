#include <poincare/round.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Round::type() const {
  return Type::Round;
}

Expression * Round::clone() const {
  Round * c = new Round(m_operands, true);
  return c;
}

bool Round::isCommutative() const {
  return false;
}

template<typename T>
Evaluation<T> * Round::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * f1Entry = operand(0)->evaluate<T>(context, angleUnit);
  Evaluation<T> * f2Entry = operand(1)->evaluate<T>(context, angleUnit);
  T f1 = f1Entry->toScalar();
  T f2 = f2Entry->toScalar();
  delete f1Entry;
  delete f2Entry;
  if (isnan(f2) || f2 != (int)f2) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T err = std::pow(10, std::floor(f2));
  return new Complex<T>(Complex<T>::Float(std::round(f1*err)/err));
}

}


