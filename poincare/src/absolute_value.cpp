#include <poincare/absolute_value.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/absolute_value_layout.h>
#include <poincare/complex_cartesian.h>
#include <poincare/multiplication.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper AbsoluteValue::s_functionHelper;

int AbsoluteValueNode::numberOfChildren() const { return AbsoluteValue::s_functionHelper.numberOfChildren(); }

Expression AbsoluteValueNode::setSign(Sign s, ReductionContext reductionContext) {
  assert(s == ExpressionNode::Sign::Positive);
  return AbsoluteValue(this);
}

Layout AbsoluteValueNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return AbsoluteValueLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int AbsoluteValueNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, AbsoluteValue::s_functionHelper.name());
}

Expression AbsoluteValueNode::shallowReduce(ReductionContext reductionContext) {
  return AbsoluteValue(this).shallowReduce(reductionContext);
}

Expression AbsoluteValue::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression e = Expression::defaultShallowReduce();
  if (e.isUndefined()) {
    return e;
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixFirstChild(reductionContext);
  }
  if (c.isReal(reductionContext.context())) {
    float app = c.node()->approximate(float(), reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit()).toScalar();
    if (!std::isnan(app) &&
        ((c.isNumber() && app >= 0) || app >= Expression::Epsilon<float>())) {
      /* abs(a) = a with a >= 0
       * To check that a > 0, if a is a number we can use float comparison;
       * in other cases, we are more conservative and rather check that
       * a > epsilon ~ 1E-7 to avoid potential error due to float precision. */
      replaceWithInPlace(c);
      return c;
    } else if (!std::isnan(app) &&
               ((c.isNumber() && app < 0.0f) || app <= -Expression::Epsilon<float>())) {
      // abs(a) = -a with a < 0 (same comment as above to check that a < 0)
      Multiplication m = Multiplication::Builder(Rational::Builder(-1), c);
      replaceWithInPlace(m);
      return m.shallowReduce(reductionContext);
    }
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Expression childNorm = complexChild.norm(reductionContext);
    replaceWithInPlace(childNorm);
    return childNorm.shallowReduce(reductionContext);
  }
  // abs(-x) = abs(x)
  c.makePositiveAnyNegativeNumeralFactor(reductionContext);
  return *this;
}

}
