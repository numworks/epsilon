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

template <typename T> Expression * Randint::templateApproximate(Context & context, AngleUnit angleUnit) const {
  Expression * aInput = operand(0)->approximate<T>(context, angleUnit);
  Expression * bInput = operand(1)->approximate<T>(context, angleUnit);
  if (aInput->type() != Type::Complex || bInput->type() != Type::Complex) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  T a = static_cast<Complex<T> *>(aInput)->toScalar();
  T b = static_cast<Complex<T> *>(bInput)->toScalar();
  delete aInput;
  delete bInput;
  if (std::isnan(a) || std::isnan(b) || a != std::round(a) || b != std::round(b) || a > b) {
    return new Complex<T>(Complex<T>::Float(NAN));

  }
  T result = std::floor(Random::random<T>()*(b+1.0-a)+a);
  return new Complex<T>(Complex<T>::Float(result));
}

}
