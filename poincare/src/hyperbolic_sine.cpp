#include <poincare/hyperbolic_sine.h>
#include <poincare/complex.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <poincare/fraction.h>
#include <poincare/opposite.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

HyperbolicSine::HyperbolicSine() :
  Function("sinh")
{
}

Expression::Type HyperbolicSine::type() const {
  return Type::HyperbolicSine;
}

Expression * HyperbolicSine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  HyperbolicSine * hs = new HyperbolicSine();
  hs->setArgument(newOperands, numberOfOperands, cloneOperands);
  return hs;
}

template<typename T>
Complex<T> HyperbolicSine::compute(const Complex<T> c) {
  if (c.b() == 0) {
    return Complex<T>::Float(std::sinh(c.a()));
  }
  Complex<T> e = Complex<T>::Float(M_E);
  Complex<T> exp1 = Power::compute(e, c);
  Complex<T> exp2 = Power::compute(e, Complex<T>::Cartesian(-c.a(), -c.b()));
  Complex<T> sub = Subtraction::compute(exp1, exp2);
  return Fraction::compute(sub, Complex<T>::Float(2));
}

}
