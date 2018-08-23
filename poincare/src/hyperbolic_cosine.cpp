#include <poincare/hyperbolic_cosine.h>
#include <poincare/addition.h>
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

Expression::Type HyperbolicCosine::type() const {
  return Type::HyperbolicCosine;
}

Expression * HyperbolicCosine::clone() const {
  HyperbolicCosine * a = new HyperbolicCosine(m_operands, true);
  return a;
}

Expression HyperbolicCosine::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
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
std::complex<T> HyperbolicCosine::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Trigonometry::RoundToMeaningfulDigits(std::cosh(c));
}

}
