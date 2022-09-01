#include <poincare/sign_function.h>
#include <poincare/symbol_abstract.h>
#include <poincare/complex_cartesian.h>
#include <poincare/float.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include <utility>

namespace Poincare {

int SignFunctionNode::numberOfChildren() const { return SignFunction::s_functionHelper.numberOfChildren(); }

Layout SignFunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(SignFunction(this), floatDisplayMode, numberOfSignificantDigits, SignFunction::s_functionHelper.aliasesList().mainAlias(), context);
}

int SignFunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SignFunction::s_functionHelper.aliasesList().mainAlias());
}

Expression SignFunctionNode::shallowReduce(const ReductionContext& reductionContext) {
  return SignFunction(this).shallowReduce(reductionContext);
}

bool SignFunctionNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return SignFunction(this).derivate(reductionContext, symbol, symbolValue);
}

template<typename T>
Complex<T> SignFunctionNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0 || std::isnan(c.real())) {
    return Complex<T>::RealUndefined();
  }
  if (c.real() == 0) {
    return Complex<T>::Builder(0.0);
  }
  if (c.real() < 0) {
    return Complex<T>::Builder(-1.0);
  }
  return Complex<T>::Builder(1.0);
}


Expression SignFunction::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::UnitReduction::KeepUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
        );
    if (!e.isUninitialized()) {
      return e;
    }
    // Discard units if any
    Expression unit;
    childAtIndex(0).removeUnit(&unit);
  }
  Expression child = childAtIndex(0);
  Rational resultSign = Rational::Builder(1);
  TrinaryBoolean s = child.isPositive(reductionContext.context());
  if (s == TrinaryBoolean::False) {
    resultSign = Rational::Builder(-1);
  } else {
    Evaluation<float> childApproximated = child.node()->approximate(1.0f, ExpressionNode::ApproximationContext(reductionContext, true));
    assert(childApproximated.type() == EvaluationNode<float>::Type::Complex);
    Complex<float> c = static_cast<Complex<float>&>(childApproximated);
    if (std::isnan(c.imag()) || std::isnan(c.real()) || c.imag() != 0) {
      // c's approximation has no sign (c is complex or NAN)
      if (reductionContext.target() == ExpressionNode::ReductionTarget::User && s == TrinaryBoolean::True) {
        // For the user, we want sign(abs(x)) = 1
      } else {
        // sign(-x) = -sign(x)
        Expression oppChild = child.makePositiveAnyNegativeNumeralFactor(reductionContext);
        if (oppChild.isUninitialized()) {
          return *this;
        }
        Expression sign = *this;
        Multiplication m = Multiplication::Builder(Rational::Builder(-1));
        replaceWithInPlace(m);
        m.addChildAtIndexInPlace(sign, 1, 1); // sign does not need to be shallowReduced because -x = NAN --> x = NAN
        return std::move(m); // m does not need to be shallowReduced, -1*sign cannot be reduced
      }
    } else if (c.real() < 0) {
      resultSign = Rational::Builder(-1);
    } else if (c.real() == 0) {
      resultSign = Rational::Builder(0);
    }
  }
  replaceWithInPlace(resultSign);
  return std::move(resultSign);
}

bool SignFunction::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  /* This function derivate is equal to 0 everywhere but in 0 where
   * it's not defined.
   * We approximate it's child to know if it is equal to 0
   * The approximation of the child might not be precise that's why it is
   * compared with EpsilonLax.
   * This derivative is used in the derivative of arccot(x)
  */
  Expression child = childAtIndex(0).clone();
  child = child.replaceSymbolWithExpression(symbol, symbolValue);
  float childValue = child.approximateToScalar<float>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
  if (std::fabs(childValue) < Float<float>::EpsilonLax()) {
    return false;
  }
  replaceWithInPlace(Rational::Builder(0));
  return true;
}

}
