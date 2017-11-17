#include <poincare/hyperbolic_arc_tangent.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicArcTangent::type() const {
  return Type::HyperbolicArcTangent;
}

Expression * HyperbolicArcTangent::clone() const {
  HyperbolicArcTangent * a = new HyperbolicArcTangent(m_operands, true);
  return a;
}

Expression * HyperbolicArcTangent::shallowReduce(Context& context, AngleUnit angleUnit) {
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
Complex<T> HyperbolicArcTangent::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::atanh(c.a()));
}

}
