#include <poincare/hyperbolic_cosine.h>
#include <poincare/complex.h>
#include <poincare/addition.h>
#include <poincare/power.h>
#include <poincare/division.h>
#include <poincare/opposite.h>
#include <poincare/simplification_engine.h>
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

Expression * HyperbolicCosine::shallowReduce(Context& context, AngleUnit angleUnit) {
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
Complex<T> HyperbolicCosine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() == 0) {
    return Complex<T>::Float(std::cosh(c.a()));
  }
  Complex<T> e = Complex<T>::Float(M_E);
  Complex<T> exp1 = Power::compute(e, c);
  Complex<T> exp2 = Power::compute(e, Complex<T>::Cartesian(-c.a(), -c.b()));
  Complex<T> sum = Addition::compute(exp1, exp2);
  return Division::compute(sum, Complex<T>::Float(2));
}

}
