#include <poincare/hyperbolic_arc_cosine.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type HyperbolicArcCosine::type() const {
  return Type::HyperbolicArcCosine;
}

Expression * HyperbolicArcCosine::clone() const {
  HyperbolicArcCosine * a = new HyperbolicArcCosine(m_operands, true);
  return a;
}

Expression * HyperbolicArcCosine::shallowReduce(Context& context, AngleUnit angleUnit) {
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
Complex<T> HyperbolicArcCosine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::acosh(c.a()));
}

}
