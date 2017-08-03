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

template<typename T>
Complex<T> HyperbolicCosine::compute(const Complex<T> c) {
  if (c.b() == 0) {
    return Complex<T>::Float(std::cosh(c.a()));
  }
  Complex<T> e = Complex<T>::Float(M_E);
  Complex<T> exp1 = Power::compute(e, c);
  Complex<T> exp2 = Power::compute(e, Complex<T>::Cartesian(-c.a(), -c.b()));
  Complex<T> sum = Addition::compute(exp1, exp2);
  return Fraction::compute(sum, Complex<T>::Float(2));
}

}
