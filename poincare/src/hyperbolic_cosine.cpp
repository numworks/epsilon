#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
#include <poincare/addition.h>
#include <poincare/power.h>
#include <poincare/fraction.h>
#include <poincare/opposite.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

HyperbolicCosine::HyperbolicCosine() :
  Function("cosh")
{
}

Expression::Type HyperbolicCosine::type() const {
  return Type::HyperbolicCosine;
}

Expression * HyperbolicCosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  HyperbolicCosine * hc = new HyperbolicCosine();
  hc->setArgument(newOperands, numberOfOperands, cloneOperands);
  return hc;
}

Complex HyperbolicCosine::compute(const Complex c) {
  if (c.b() == 0.0f) {
    return Complex::Float(std::cosh(c.a()));
  }
  Complex e = Complex::Float(M_E);
  Complex exp1 = Power::compute(e, c);
  Complex exp2 = Power::compute(e, Complex::Cartesian(-c.a(), -c.b()));
  Complex sum = Addition::compute(exp1, exp2);
  return Fraction::compute(sum, Complex::Float(2.0f));
}

}
