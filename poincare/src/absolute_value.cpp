#include <poincare/absolute_value.h>
#include <poincare/constant.h>
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

Expression AbsoluteValueNode::getUnit() const {
  return childAtIndex(0)->getUnit();
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
  e = e.defaultHandleUnitsInChildren();
  if (e.isUndefined()) {
    return e;
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::Matrix) {
    return mapOnMatrixFirstChild(reductionContext);
  }
  if (c.deepIsMatrix(reductionContext.context())) {
    return *this;
  }
  // |x| = ±x if x is real
  if (c.isReal(reductionContext.context())) {
    double app = c.node()->approximate(double(), reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit()).toScalar();
    if (!std::isnan(app)) {
      if ((c.isNumber() && app >= 0) || app >= Expression::Epsilon<double>()) {
        /* abs(a) = a with a >= 0
         * To check that a > 0, if a is a number we can use float comparison;
         * in other cases, we are more conservative and rather check that
         * a > epsilon ~ 1E-7 to avoid potential error due to float precision. */
        replaceWithInPlace(c);
        return c;
      } else if ((c.isNumber() && app < 0.0f) || app <= -Expression::Epsilon<double>()) {
        // abs(a) = -a with a < 0 (same comment as above to check that a < 0)
        Multiplication m = Multiplication::Builder(Rational::Builder(-1), c);
        replaceWithInPlace(m);
        return m.shallowReduce(reductionContext);
      }
    }
  }
  // |a+ib| = sqrt(a^2+b^2)
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Expression childNorm = complexChild.norm(reductionContext);
    replaceWithInPlace(childNorm);
    return childNorm.shallowReduce(reductionContext);
  }
  // |z^y| = |z|^y if y is real
  /* Proof:
   * Let's write z = r*e^(i*θ) and y = a+ib
   * |z^y| = |(r*e^(i*θ))^(a+ib)|
   *       = |r^a*r^(i*b)*e^(i*θ*a)*e^(-θ*b)|
   *       = |r^a*e^(i*b*ln(r))*e^(i*θ*a)*e^(-θ*b)|
   *       = |r^a*e^(-θ*b)|
   *       = r^a*|e^(-θ*b)|
   * |z|^y = |r*e^(i*θ)|^(a+ib)
   *       = r^(a+ib)
   *       = r^a*r^(i*b)
   *       = r^a*e^(i*b*ln(r))
   * So if b = 0, |z^y| = |z|^y
   */
  if (c.type() == ExpressionNode::Type::Power && c.childAtIndex(1).isReal(reductionContext.context())) {
    Expression newabs = AbsoluteValue::Builder(c.childAtIndex(0));
    c.replaceChildAtIndexInPlace(0, newabs);
    newabs.shallowReduce(reductionContext);
    replaceWithInPlace(c);
    return c.shallowReduce(reductionContext);
  }
  // |x*y| = |x|*|y|
  if (c.type() == ExpressionNode::Type::Multiplication) {
    Multiplication m = Multiplication::Builder();
    for (int i = 0; i < c.numberOfChildren(); i++) {
      AbsoluteValue newabs  = AbsoluteValue::Builder(c.childAtIndex(i));
      m.addChildAtIndexInPlace(newabs, m.numberOfChildren(), m.numberOfChildren());
      newabs.shallowReduce(reductionContext);
    }
    replaceWithInPlace(m);
    return m.shallowReduce(reductionContext);
  }
 // |i| = 1
  if (c.type() == ExpressionNode::Type::Constant && static_cast<const Constant &>(c).isIComplex()) {
    Expression e = Rational::Builder(1);
    replaceWithInPlace(e);
    return e;
  }
  // abs(-x) = abs(x)
  c.makePositiveAnyNegativeNumeralFactor(reductionContext);
  return *this;
}

}
