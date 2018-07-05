#include <poincare/hyperbolic_arc_sine.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicArcSine::type() const {
  return Type::HyperbolicArcSine;
}

Expression * HyperbolicArcSine::clone() const {
  HyperbolicArcSine * a = new HyperbolicArcSine(m_operands, true);
  return a;
}

Expression * HyperbolicArcSine::shallowReduce(Context& context, AngleUnit angleUnit) {
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
  return this;
}

template<typename T>
std::complex<T> HyperbolicArcSine::computeOnComplex(const std::complex<T> c, AngleUnit angleUnit) {
  return std::asinh(c);
}

}
