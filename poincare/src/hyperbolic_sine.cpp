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

Complex HyperbolicSine::compute(const Complex c) {
  if (c.b() == 0.0f) {
    return Complex::Float(std::sinh(c.a()));
  }
  Complex e = Complex::Float(M_E);
  Complex exp1 = Power::compute(e, c);
  Complex exp2 = Power::compute(e, Complex::Cartesian(-c.a(), -c.b()));
  Complex sub = Subtraction::compute(exp1, exp2);
  return Fraction::compute(sub, Complex::Float(2.0f));
}

}
