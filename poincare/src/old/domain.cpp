#include <poincare/old/domain.h>

namespace Poincare {

OMG::Troolean Domain::ExpressionIsIn(const OExpression &expression, Type type,
                                     Context *context) {
  if (expression.deepIsMatrix(context)) {
    return OMG::Troolean::False;
  }

  if (expression.isUndefined() || OExpression::IsInfinity(expression)) {
    return OMG::Troolean::False;
  }

  if (type & k_onlyPositive) {
    OMG::Troolean isPositive = expression.isPositive(context);
    if (isPositive != OMG::Troolean::True) {
      return isPositive;
    }
  }

  if (type & k_onlyNegative) {
    OMG::Troolean isPositive = expression.isPositive(context);
    if (isPositive != OMG::Troolean::False) {
      return isPositive == OMG::Troolean::True ? OMG::Troolean::False
                                               : OMG::Troolean::Unknown;
    }
  }

  if (!expression.isReal(context, false)) {
    return OMG::Troolean::Unknown;
  }

  if (expression.otype() != ExpressionNode::Type::Rational) {
    return OMG::Troolean::Unknown;
  }

  const Rational rational = static_cast<const Rational &>(expression);

  if (type & k_onlyIntegers && !rational.isInteger()) {
    return OMG::Troolean::False;
  }

  if (type & k_nonZero && rational.isZero()) {
    return OMG::Troolean::False;
  }

  if (type & (UnitSegment | LeftOpenUnitSegment | OpenUnitSegment) &&
      rational.isGreaterThanOne()) {
    return OMG::Troolean::False;
  }

  return OMG::Troolean::True;
}

}  // namespace Poincare
