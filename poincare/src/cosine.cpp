#include <poincare/cosine.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Cosine::Cosine() :
  Function("cos")
{
}

Expression::Type Cosine::type() const {
  return Type::Cosine;
}

Expression * Cosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  Cosine * c = new Cosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

template<typename T>
Complex<T> Cosine::compute(const Complex<T> c, AngleUnit angleUnit) {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() == 0) {
    T input = c.a();
    if (angleUnit == AngleUnit::Degree) {
      input *= M_PI/180.0f;
    }
    T result = std::cos(input);
    /* Cheat: openbsd trigonometric functions (cos, sin & tan) are numerical
     * implementation and thus are approximative. The error epsilon is ~1E-7
     * on float and ~1E-15 on double. In order to avoid weird results as
     * cos(90) = 6E-17, we neglect the result when its ratio with the argument
     * (pi in the exemple) is smaller than epsilon.
     * We can't do that for all evaluation as the user can operate on values as
     * small as 1E-308 (in double) and most results still be correct. */
    if (input !=  0 && std::fabs(result/input) <= epsilon<T>()) {
      return Complex<T>::Float(0);
    }
    return Complex<T>::Float(result);
  }
  Complex<T> arg = Complex<T>::Cartesian(-c.b(), c.a());
  return HyperbolicCosine::compute(arg);
}

}
