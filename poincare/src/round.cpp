#include <poincare/round.h>
#include <poincare/undefined.h>

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

Expression * Round::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  if (operand(0)->type() == Type::Matrix || operand(1)->type() == Type::Matrix) {
    return replaceWith(new Undefined(), true);
  }
  return this; // TODO: implement for rationals!
}

template<typename T>
Complex<T> * Round::templatedEvaluate(Context& context, AngleUnit angleUnit) const {
  Complex<T> * f1Entry = operand(0)->privateEvaluate(T(), context, angleUnit);
  Complex<T> * f2Entry = operand(1)->privateEvaluate(T(), context, angleUnit);
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


