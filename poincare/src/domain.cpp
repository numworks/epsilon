#include <poincare/domain.h>

namespace Poincare {

static Domain::Result isGreaterThanOne(const Rational &rational, Context * context) {
  Integer num = rational.unsignedIntegerNumerator();
  Integer den = rational.integerDenominator();
  if (den.isLowerThan(num)) {
    return Domain::False;
  }

  return Domain::True;
}

Domain::Result Domain::expressionIsIn(const Expression &expression, Type type, Context * context) {
  assert(result != nullptr);
  Result res;
  if (expression.deepIsMatrix(context)) {
    return False;
  }

  if (expression.isUndefined() || Expression::IsInfinity(expression, context)) {
    return False;
  }

  if (type & onlyPositive) {
    ExpressionNode::Sign sign = expression.sign(context);
    if (sign == ExpressionNode::Sign::Negative) {
      return False;
    }
    if (sign == ExpressionNode::Sign::Unknown) {
      return CantCheck;
    }
  }

  if (type & onlyNegative) {
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

  if (type & onlyIntegers && !rational.isInteger()) {
    return False;
  }

  if (type & nonZero && rational.isZero()) {
    return False;
  }

  if (type & (UnitSegment | LeftOpenUnitSegment | OpenUnitSegment) && (res=isGreaterThanOne(rational, context))) {
    return res;
  }

  return True;
}

}
