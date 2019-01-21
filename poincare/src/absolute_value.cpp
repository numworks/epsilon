#include <poincare/absolute_value.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/absolute_value_layout.h>
#include <poincare/complex_cartesian.h>
#include <poincare/multiplication.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper AbsoluteValue::s_functionHelper;

int AbsoluteValueNode::numberOfChildren() const { return AbsoluteValue::s_functionHelper.numberOfChildren(); }

Expression AbsoluteValueNode::setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  assert(s == ExpressionNode::Sign::Positive);
  return AbsoluteValue(this).setSign(s, context, complexFormat, angleUnit);
}

Layout AbsoluteValueNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return AbsoluteValueLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int AbsoluteValueNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, AbsoluteValue::s_functionHelper.name());
}

Expression AbsoluteValueNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return AbsoluteValue(this).shallowReduce(context, complexFormat, angleUnit, target);
}

Expression AbsoluteValue::setSign(ExpressionNode::Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  assert(s == ExpressionNode::Sign::Positive);
  return *this;
}

Expression AbsoluteValue::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression e = Expression::defaultShallowReduce();
  if (e.isUndefined()) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
#if 0
  if (c->type() == Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
#endif
#endif
  Expression c = childAtIndex(0);
  if (c.isReal(context)) {
    float app = c.node()->approximate(float(), context, complexFormat, angleUnit).toScalar();
    if (!std::isnan(app) && app >= Expression::Epsilon<float>()) {
      // abs(a) = a with a > 0
      replaceWithInPlace(c);
      return c;
    } else if (!std::isnan(app) && app <= -Expression::Epsilon<float>()) {
      // abs(a) = -a with a < 0
      Multiplication m(Rational(-1), c);
      replaceWithInPlace(m);
      return m.shallowReduce(context, complexFormat, angleUnit, target);
    }
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Expression childNorm = complexChild.norm(context, complexFormat, angleUnit, target);
    replaceWithInPlace(childNorm);
    return childNorm.shallowReduce(context, complexFormat, angleUnit, target);
  }
  // abs(-x) = abs(x)
  c.makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
  return *this;
}

}
