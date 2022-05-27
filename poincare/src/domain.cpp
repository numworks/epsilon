#include <poincare/domain.h>

namespace Poincare {

Domain::Result Domain::ExpressionIsIn(const Expression &expression, Type type, Context * context) {
  if (expression.deepIsMatrix(context)) {
    return False;
  }

  if (expression.isUndefined() || Expression::IsInfinity(expression, context)) {
    return False;
  }

  if (type & k_onlyPositive) {
    ExpressionNode::Sign sign = expression.sign(context);
    if (sign == ExpressionNode::Sign::Negative) {
      return False;
    }
    if (sign == ExpressionNode::Sign::Unknown) {
      return CantCheck;
    }
  }

  if (type & k_onlyNegative) {
    ExpressionNode::Sign sign = expression.sign(context);
    if (sign == ExpressionNode::Sign::Positive) {
      return False;
    }
    if (sign == ExpressionNode::Sign::Unknown) {
      return CantCheck;
    }
  }

  if (!expression.isReal(context)) {
    return CantCheck;
  }

  if (expression.type() != ExpressionNode::Type::Rational) {
    return CantCheck;
  }

  const Rational rational = static_cast<const Rational &>(expression);

  if (type & k_onlyIntegers && !rational.isInteger()) {
    return False;
  }

  if (type & k_nonZero && rational.isZero()) {
    return False;
  }

  if (type & (UnitSegment | LeftOpenUnitSegment | OpenUnitSegment) && rational.isGreaterThanOne()) {
    return False;
  }

  return True;
}

}
