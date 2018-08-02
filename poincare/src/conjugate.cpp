#include <poincare/conjugate.h>
#include <poincare/simplification_engine.h>
#include <poincare/conjugate_layout_node.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

Expression::Type Conjugate::type() const {
  return Type::Conjugate;
}

Expression * Conjugate::clone() const {
  Conjugate * a = new Conjugate(m_operands, true);
  return a;
}

LayoutRef Conjugate::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return ConjugateLayoutRef(operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

ExpressionReference Conjugate::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  if (op->type() == Type::Rational) {
    return replaceWith(op, true);
  }
  return this;
}

template<typename T>
std::complex<T> Conjugate::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return std::conj(c);
}

}

