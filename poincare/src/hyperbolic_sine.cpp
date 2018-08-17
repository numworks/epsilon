#include <poincare/hyperbolic_sine.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <poincare/division.h>
#include <poincare/opposite.h>
#include <poincare/simplification_helper.h>
#include <poincare/trigonometry.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicSine::type() const {
  return Type::HyperbolicSine;
}

Expression * HyperbolicSine::clone() const {
  HyperbolicSine * a = new HyperbolicSine(m_operands, true);
  return a;
}

Expression HyperbolicSine::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  Expression * e = Expression::defaultShallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  Expression * op = editableOperand(0);
  if (op->type() == Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
#endif
  return this;
}

template<typename T>
std::complex<T> HyperbolicSine::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Trigonometry::RoundToMeaningfulDigits(std::sinh(c));
}

}
