#include <poincare/absolute_value.h>
#include <poincare/constant.h>
#include <poincare/dependency.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/absolute_value_layout.h>
#include <poincare/complex_cartesian.h>
#include <poincare/float.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/derivative.h>
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

int AbsoluteValueNode::numberOfChildren() const { return AbsoluteValue::s_functionHelper.numberOfChildren(); }

Layout AbsoluteValueNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return AbsoluteValueLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

int AbsoluteValueNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, AbsoluteValue::s_functionHelper.aliasesList().mainAlias());
}

Expression AbsoluteValueNode::shallowReduce(const ReductionContext& reductionContext) {
  return AbsoluteValue(this).shallowReduce(reductionContext);
}

bool AbsoluteValueNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return AbsoluteValue(this).derivate(reductionContext, symbol, symbolValue);
}

Expression AbsoluteValue::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::ExtractUnitsOfFirstChild,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);

  // |x| = ±x if x is real
  if (c.isReal(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
    double app = c.node()->approximate(double(), ExpressionNode::ApproximationContext(reductionContext, true)).toScalar();
    if (!std::isnan(app)) {
      if ((c.isNumber() && app >= 0) || app >= Float<double>::EpsilonLax()) {
        /* abs(a) = a with a >= 0
         * To check that a > 0, if a is a number we can use float comparison;
         * in other cases, we are more conservative and rather check that
         * a > epsilon ~ 1E-7 to avoid potential error due to float precision. */
        replaceWithInPlace(c);
        return c;
      } else if ((c.isNumber() && app < 0.0f) || app <= -Float<double>::EpsilonLax()) {
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
  if (c.type() == ExpressionNode::Type::Power && c.childAtIndex(1).isReal(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
    List listOfDependencies = List::Builder();
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      listOfDependencies.addChildAtIndexInPlace(c.clone(), 0, 0);
    }
    Expression newabs = AbsoluteValue::Builder(c.childAtIndex(0));
    c.replaceChildAtIndexInPlace(0, newabs);
    newabs.shallowReduce(reductionContext);
    if (reductionContext.complexFormat() == Preferences::ComplexFormat::Real) {
      c = Dependency::Builder(c.shallowReduce(reductionContext), listOfDependencies);
    }
    replaceWithInPlace(c);
    return c.shallowReduce(reductionContext);
  }
  // |x*y| = |x|*|y|
  if (c.type() == ExpressionNode::Type::Multiplication) {
    Multiplication m = Multiplication::Builder();
    int childrenNumber = c.numberOfChildren();
    for (int i = 0; i < childrenNumber; i++) {
      AbsoluteValue newabs  = AbsoluteValue::Builder(c.childAtIndex(i));
      m.addChildAtIndexInPlace(newabs, m.numberOfChildren(), m.numberOfChildren());
      newabs.shallowReduce(reductionContext);
    }
    replaceWithInPlace(m);
    return m.shallowReduce(reductionContext);
  }
 // |i| = 1
  if (c.type() == ExpressionNode::Type::ConstantMaths && static_cast<const Constant &>(c).isConstant("i")) {
    Expression e = Rational::Builder(1);
    replaceWithInPlace(e);
    return e;
  }
  // abs(-x) = abs(x)
  c.makePositiveAnyNegativeNumeralFactor(reductionContext);
  return *this;
}

// Derivate of |f(x)| is f'(x)*sg(x) (and undef in 0) = f'(x)*(f(x)/|f(x)|)
bool AbsoluteValue::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  Expression f = childAtIndex(0);
  Multiplication result = Multiplication::Builder();
  result.addChildAtIndexInPlace(Derivative::Builder(f.clone(),
    symbol.clone().convert<Symbol>(),
    symbolValue.clone()
    ),0,0);
  result.addChildAtIndexInPlace(f.clone(),1,1);
  replaceWithInPlace(result);
  result.addChildAtIndexInPlace(Power::Builder(*this,Rational::Builder(-1)),2,2);
  return true;
}

}
