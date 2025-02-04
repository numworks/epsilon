#include <poincare/old/hyperbolic_trigonometric_function.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/rational.h>
#include <poincare/old/simplification_helper.h>

namespace Poincare {

OExpression HyperbolicTrigonometricFunctionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return HyperbolicTrigonometricFunction(this).shallowReduce(reductionContext);
}

OExpression HyperbolicTrigonometricFunction::shallowReduce(
    ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  OExpression thisExpression = *this;
  OExpression c = childAtIndex(0);

  // Step 1. Notable values
  if (node()->isNotableValue(c, reductionContext.context())) {
    OExpression result = node()->imageOfNotableValue();
    replaceWithInPlace(result);
    return result;
  }

  // Step 2. Look for an expression of type "cosh(acosh(z))", return z
  ExpressionNode::Type t = otype();
  ExpressionNode::Type childT = c.otype();
  ApproximationContext approximationContext(reductionContext, true);
  {
    OExpression result;
    if (t == ExpressionNode::Type::HyperbolicCosine) {
      if (childT == ExpressionNode::Type::HyperbolicArcCosine) {
        OExpression e = childAtIndex(0).childAtIndex(0);
        if (reductionContext.complexFormat() !=
                Preferences::ComplexFormat::Real ||
            (e.isReal(reductionContext.context(),
                      reductionContext.shouldCheckMatrices()) &&
             e.approximateToRealScalar<double>(approximationContext) >= 1.0)) {
          result = e;
        }
      }
    } else if (t == ExpressionNode::Type::HyperbolicArcCosine) {
      if (childT == ExpressionNode::Type::HyperbolicCosine) {
        OExpression e = childAtIndex(0).childAtIndex(0);
        if (reductionContext.complexFormat() !=
                Preferences::ComplexFormat::Real ||
            (e.isReal(reductionContext.context(),
                      reductionContext.shouldCheckMatrices()) &&
             e.approximateToRealScalar<double>(approximationContext) >= 0.0)) {
          result = e;
        }
      }
    } else if (t == ExpressionNode::Type::HyperbolicSine) {
      if (childT == ExpressionNode::Type::HyperbolicArcSine) {
        result = childAtIndex(0).childAtIndex(0);
      }
    } else if (t == ExpressionNode::Type::HyperbolicArcSine) {
      if (childT == ExpressionNode::Type::HyperbolicSine) {
        result = childAtIndex(0).childAtIndex(0);
      }
    } else if (t == ExpressionNode::Type::HyperbolicTangent) {
      if (childT == ExpressionNode::Type::HyperbolicArcTangent) {
        OExpression e = childAtIndex(0).childAtIndex(0);
        if (reductionContext.complexFormat() !=
                Preferences::ComplexFormat::Real ||
            (e.isReal(reductionContext.context(),
                      reductionContext.shouldCheckMatrices()) &&
             std::fabs(e.approximateToRealScalar<double>(
                 approximationContext)) < 1.0)) {
          result = e;
        }
      }
    } else {
      assert(t == ExpressionNode::Type::HyperbolicArcTangent);
      if (childT == ExpressionNode::Type::HyperbolicTangent) {
        result = childAtIndex(0).childAtIndex(0);
      }
    }
    if (!result.isUninitialized()) {
      replaceWithInPlace(result);
      return result;
    }
  }

  // Step 3. Look for an expression of type "cosh(-x)", return "+/-cosh(x)"
  if (t != ExpressionNode::Type::HyperbolicArcCosine) {
    OExpression positiveArg =
        childAtIndex(0).makePositiveAnyNegativeNumeralFactor(reductionContext);
    if (!positiveArg.isUninitialized()) {
      // The argument was of form cosh(-a)
      if (t == ExpressionNode::Type::HyperbolicCosine) {
        // cosh(-a) = cosh(a)
        return shallowReduce(reductionContext);
      } else {
        /* sinh(-a) = -sinh(a) or tanh(-a) = -tanh(a) or arcsinh(-a) =
         * -arcsinh(a) or arctanh(-a) = -arctanh(a) */
        assert(t == ExpressionNode::Type::HyperbolicSine ||
               t == ExpressionNode::Type::HyperbolicArcSine ||
               t == ExpressionNode::Type::HyperbolicTangent ||
               t == ExpressionNode::Type::HyperbolicArcTangent);
        Multiplication m = Multiplication::Builder(Rational::Builder(-1));
        thisExpression.replaceWithInPlace(m);
        m.addChildAtIndexInPlace(thisExpression, 1, 1);
        thisExpression.shallowReduce(reductionContext);
        return m.shallowReduce(reductionContext);
      }
    }
  }
  return *this;
}

}  // namespace Poincare
