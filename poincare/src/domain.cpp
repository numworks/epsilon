#include <poincare/domain.h>

namespace Poincare {

TrinaryBoolean Domain::ExpressionIsIn(const Expression &expression, Type type,
                                      Context *context) {
  if (expression.deepIsMatrix(context)) {
    return TrinaryBoolean::False;
  }

  if (expression.isUndefined() || Expression::IsInfinity(expression, context)) {
    return TrinaryBoolean::False;
  }

  if (type & k_onlyPositive) {
    TrinaryBoolean isPositive = expression.isPositive(context);
    if (isPositive != TrinaryBoolean::True) {
      return isPositive;
    }
  }

  if (type & k_onlyNegative) {
    TrinaryBoolean isPositive = expression.isPositive(context);
    if (isPositive != TrinaryBoolean::False) {
      return isPositive == TrinaryBoolean::True ? TrinaryBoolean::False
                                                : TrinaryBoolean::Unknown;
    }
  }

  if (!expression.isReal(context, false)) {
    return TrinaryBoolean::Unknown;
  }

  if (expression.type() != ExpressionNode::Type::Rational) {
    return TrinaryBoolean::Unknown;
  }

  const Rational rational = static_cast<const Rational &>(expression);

  if (type & k_onlyIntegers && !rational.isInteger()) {
    return TrinaryBoolean::False;
  }

  if (type & k_nonZero && rational.isZero()) {
    return TrinaryBoolean::False;
  }

  if (type & (UnitSegment | LeftOpenUnitSegment | OpenUnitSegment) &&
      rational.isGreaterThanOne()) {
    return TrinaryBoolean::False;
  }

  return TrinaryBoolean::True;
}

}  // namespace Poincare
