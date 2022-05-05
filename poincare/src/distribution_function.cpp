#include <poincare/distribution_function.h>
#include <poincare/binomial_distribution.h>
#include <poincare/simplification_helper.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>
#include <assert.h>

namespace Poincare {

Layout DistributionFunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(DistributionFunction(this), floatDisplayMode, numberOfSignificantDigits, name());
}

int DistributionFunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
}

Expression DistributionFunctionNode::shallowReduce(ReductionContext reductionContext) {
  return DistributionFunction(this).shallowReduce(reductionContext.context());
}

Expression DistributionFunction::shallowReduce(Context * context, bool * stopReduction) {
  if (stopReduction != nullptr) {
    *stopReduction = true;
  }
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  int childIndex = numberOfParameters(functionType());
  Expression parameters[Distribution::maxNumberOfParameters];
  for (int i=0; i < Distribution::numberOfParameters(distributionType()); i++) {
    parameters[i] = childAtIndex(childIndex++);
  }
  BinomialDistribution distributionPlaceholder;
  Distribution * distribution = &distributionPlaceholder;
  Distribution::Initialize(distribution, distributionType());

  bool parametersAreOk;
  bool couldCheckParameters = distribution->ExpressionParametersAreOK(&parametersAreOk, parameters, context);

  if (!couldCheckParameters) {
    return *this;
  }
  if (!parametersAreOk) {
    return replaceWithUndefinedInPlace();
  }

  switch (functionType()) {
  case FunctionType::Inverse:
  {
      Expression a = childAtIndex(0);
      // Check a
      if (a.deepIsMatrix(context)) {
        return replaceWithUndefinedInPlace();
      }
      if (a.type() != ExpressionNode::Type::Rational) {
        return *this;
      }

      // Special values

      // Undef if a < 0 or a > 1
      Rational rationalA = static_cast<Rational &>(a);
      if (rationalA.isNegative()) {
        return replaceWithUndefinedInPlace();
      }
      Integer num = rationalA.unsignedIntegerNumerator();
      Integer den = rationalA.integerDenominator();
      if (den.isLowerThan(num)) {
        return replaceWithUndefinedInPlace();
      }
      bool is0 = rationalA.isZero();
      bool is1 = !is0 && rationalA.isOne();
      if (is0 || is1) {
        Expression result = Infinity::Builder(is0);
        replaceWithInPlace(result);
        return result;
      }
  }
  default:
    ;
  }

  if (stopReduction != nullptr) {
    *stopReduction = false;
  }
  return *this;
}


template<typename T>
Evaluation<T> DistributionFunctionNode::templatedApproximate(ApproximationContext approximationContext) const {
  int childIndex = 0;
  T x, y;
  switch (m_functionType) {
      case FunctionType::CDF:
      case FunctionType::PDF:
      case FunctionType::Inverse:
      {
        Evaluation<T> xEvaluation = childAtIndex(childIndex++)->approximate(T(), approximationContext);
        x = xEvaluation.toScalar();
        break;
      }
      case FunctionType::CDFRange:
      {
        Evaluation<T> xEvaluation = childAtIndex(childIndex++)->approximate(T(), approximationContext);
        Evaluation<T> yEvaluation = childAtIndex(childIndex++)->approximate(T(), approximationContext);
        x = xEvaluation.toScalar();
        y = yEvaluation.toScalar();
        break;
      }
  }
  T result = NAN;
  // Distributions are only vpointers
  BinomialDistribution distributionPlaceholder;
  Distribution * distribution = &distributionPlaceholder;
  Distribution::Initialize(distribution, m_distributionType);
  T parameters[Distribution::maxNumberOfParameters];
  for (int i=0; i < Distribution::numberOfParameters(m_distributionType); i++) {
    Evaluation<T> evaluation = childAtIndex(childIndex++)->approximate(T(), approximationContext);
    parameters[i] = evaluation.toScalar();
  }
  switch (m_functionType) {
  case FunctionType::PDF:
    result = distribution->EvaluateAtAbscissa(x, parameters);
    break;
  case FunctionType::CDF:
    result = distribution->CumulativeDistributiveFunctionAtAbscissa(x, parameters);
    break;
  case FunctionType::Inverse:
    result = distribution->CumulativeDistributiveInverseForProbability(x, parameters);
    break;
  case FunctionType::CDFRange:
    result = distribution->CumulativeDistributiveFunctionForRange(x, y, parameters);
    break;
  }
  return Complex<T>::Builder(result);
}
}
