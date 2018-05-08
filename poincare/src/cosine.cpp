#include <poincare/cosine.h>
#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
#include <poincare/symbol.h>
#include <poincare/rational.h>
#include <poincare/multiplication.h>
#include <poincare/simplification_engine.h>
#include <ion.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Cosine::type() const {
  return Type::Cosine;
}

Expression * Cosine::clone() const {
  Cosine * a = new Cosine(m_operands, true);
  return a;
}

float Cosine::characteristicXRange(Context & context, AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(this, context, angleUnit);
}

Expression * Cosine::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  return Trigonometry::shallowReduceDirectFunction(this, context, angleUnit);
}

template<typename T>
Complex<T> Cosine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
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
  return HyperbolicCosine::computeOnComplex(arg, angleUnit);
}

}
