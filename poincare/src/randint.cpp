#include <poincare/randint.h>
#include <poincare/random.h>
#include <ion.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Randint::type() const {
  return Type::Randint;
}

Expression * Randint::clone() const {
  Randint * a = new Randint(m_operands, true);
  return a;
}

template <typename T> Evaluation<T> * Randint::templateApproximate(Context & context, AngleUnit angleUnit) const {
  Evaluation<T> * aInput = operand(0)->privateApproximate(T(), context, angleUnit);
  Evaluation<T> * bInput = operand(1)->privateApproximate(T(), context, angleUnit);
  T a = aInput->toScalar();
  T b = bInput->toScalar();
  delete aInput;
  delete bInput;
  if (std::isnan(a) || std::isnan(b) || a != std::round(a) || b != std::round(b) || a > b) {
    return new Complex<T>(Complex<T>::Undefined());

  }
  T result = std::floor(Random::random<T>()*(b+1.0-a)+a);
  return new Complex<T>(result);
}

}
