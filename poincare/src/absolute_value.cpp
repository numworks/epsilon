#include <poincare/absolute_value.h>
#include <poincare/simplification_helper.h>
#include <poincare/absolute_value_layout_node.h>

extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type AbsoluteValue::type() const {
  return Type::AbsoluteValue;
}

Expression * AbsoluteValue::clone() const {
  AbsoluteValue * a = new AbsoluteValue(m_operands, true);
  return a;
}

Expression * AbsoluteValue::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) const {
  assert(s == Sign::Positive);
  return this;
}

LayoutRef AbsoluteValue::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return AbsoluteValueLayoutRef(operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

Expression AbsoluteValue::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  Expression * e = Expression::defaultShallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  Expression * op = editableOperand(0);
#if MATRIX_EXACT_REDUCING
  if (op->type() == Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
#endif
  if (op->sign() == Sign::Positive) {
    return replaceWith(op, true);
  }
  if (op->sign() == Sign::Negative) {
    Expression * newOp = op->setSign(Sign::Positive, context, angleUnit);
    return replaceWith(newOp, true);
  }
  return this;
}

template<typename T>
std::complex<T> AbsoluteValue::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return std::abs(c);
}

}
