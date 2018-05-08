#include <poincare/tangent.h>
#include <poincare/complex.h>
#include <poincare/sine.h>
#include <poincare/cosine.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/trigonometry.h>
#include <poincare/hyperbolic_tangent.h>
#include <poincare/simplification_engine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type Tangent::type() const {
  return Expression::Type::Tangent;
}

Expression * Tangent::clone() const {
  Tangent * a = new Tangent(m_operands, true);
  return a;
}

float Tangent::characteristicXRange(Context & context, AngleUnit angleUnit) const {
  return Trigonometry::characteristicXRange(this, context, angleUnit);
}

Expression * Tangent::shallowReduce(Context& context, AngleUnit angleUnit) {
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
  Expression * newExpression = Trigonometry::shallowReduceDirectFunction(this, context, angleUnit);
  if (newExpression->type() == Type::Tangent) {
    const Expression * op[1] = {newExpression->operand(0)};
    Sine * s = new Sine(op, true);
    Cosine * c = new Cosine(op, true);
    Division * d = new Division(s, c, false);
    newExpression = newExpression->replaceWith(d, true);
    return newExpression->shallowReduce(context, angleUnit);
  }
  return newExpression;
}

template<typename T>
Complex<T> Tangent::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  Complex<T> result = Division::compute(Sine::computeOnComplex(c, angleUnit), Cosine::computeOnComplex(c, angleUnit));
  if (!std::isnan(result.a()) && !std::isnan(result.b())) {
    return result;
  }
  Complex<T> tanh = HyperbolicTangent::computeOnComplex(Multiplication::compute(Complex<T>::Cartesian(0, -1), c), angleUnit);
  return Multiplication::compute(Complex<T>::Cartesian(0, 1), tanh);
}

}
