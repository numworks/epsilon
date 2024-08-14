#include <assert.h>
#include <omg/float.h>
#include <poincare/layout.h>
#include <poincare/old/absolute_value.h>
#include <poincare/old/complex_argument.h>
#include <poincare/old/complex_cartesian.h>
#include <poincare/old/constant.h>
#include <poincare/old/dependency.h>
#include <poincare/old/derivative.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/power.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/sign_function.h>
#include <poincare/old/simplification_helper.h>

#include <cmath>

namespace Poincare {

int AbsoluteValueNode::numberOfChildren() const {
  return AbsoluteValue::s_functionHelper.numberOfChildren();
}

size_t AbsoluteValueNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      AbsoluteValue::s_functionHelper.aliasesList().mainAlias());
}

OExpression AbsoluteValueNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return AbsoluteValue(this).shallowReduce(reductionContext);
}

bool AbsoluteValueNode::derivate(const ReductionContext& reductionContext,
                                 Symbol symbol, OExpression symbolValue) {
  return false;
}

OExpression AbsoluteValue::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::ExtractUnitsOfFirstChild,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);

  // |x| = ±x if x is real
  if (c.isReal(reductionContext.context(),
               reductionContext.shouldCheckMatrices()) ||
      reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
    double app = c.node()
                     ->approximate(double(),
                                   ApproximationContext(reductionContext, true))
                     .toScalar();
    if (!std::isnan(app)) {
      if ((c.isNumber() && app >= 0) ||
          app >= OMG::Float::EpsilonLax<double>()) {
        /* abs(a) = a with a >= 0
         * To check that a > 0, if a is a number we can use float comparison;
         * in other cases, we are more conservative and rather check that
         * a > epsilon ~ 1E-7 to avoid potential error due to float precision.
         */
        replaceWithInPlace(c);
        return c;
      } else if ((c.isNumber() && app < 0.0f) ||
                 app <= -OMG::Float::EpsilonLax<double>()) {
        // abs(a) = -a with a < 0 (same comment as above to check that a < 0)
        Multiplication m = Multiplication::Builder(Rational::Builder(-1), c);
        replaceWithInPlace(m);
        return m.shallowReduce(reductionContext);
      }
    } else if (reductionContext.target() != ReductionTarget::User) {
      // Do not display sign(x)*x to the user
      OExpression sign = SignFunction::Builder(c.clone());
      OExpression result = Multiplication::Builder(sign, c);
      sign.shallowReduce(reductionContext);
      replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
  }

  // |a+ib| = sqrt(a^2+b^2)
  if (c.otype() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian&>(c);
    OExpression childNorm = complexChild.norm(reductionContext);
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
  if (c.otype() == ExpressionNode::Type::Power &&
      c.childAtIndex(1).isReal(reductionContext.context(),
                               reductionContext.shouldCheckMatrices())) {
    OList listOfDependencies = OList::Builder();
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      listOfDependencies.addChildAtIndexInPlace(c.clone(), 0, 0);
    }
    OExpression newabs = AbsoluteValue::Builder(c.childAtIndex(0));
    c.replaceChildAtIndexInPlace(0, newabs);
    newabs.shallowReduce(reductionContext);
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      c = Dependency::Builder(c.shallowReduce(reductionContext),
                              listOfDependencies);
    }
    replaceWithInPlace(c);
    return c.shallowReduce(reductionContext);
  }

  // |x*y| = |x|*|y|
  if (c.otype() == ExpressionNode::Type::Multiplication) {
    Multiplication m = Multiplication::Builder();
    int childrenNumber = c.numberOfChildren();
    for (int i = 0; i < childrenNumber; i++) {
      AbsoluteValue newabs = AbsoluteValue::Builder(c.childAtIndex(i));
      m.addChildAtIndexInPlace(newabs, m.numberOfChildren(),
                               m.numberOfChildren());
      newabs.shallowReduce(reductionContext);
    }
    replaceWithInPlace(m);
    return m.shallowReduce(reductionContext);
  }

  // |i| = 1
  if (c.otype() == ExpressionNode::Type::ConstantMaths &&
      static_cast<const Constant&>(c).isComplexI()) {
    OExpression e = Rational::Builder(1);
    replaceWithInPlace(e);
    return e;
  }

  // abs(-x) = abs(x)
  c.makePositiveAnyNegativeNumeralFactor(reductionContext);
  return *this;
}

}  // namespace Poincare
