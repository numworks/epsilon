#include <poincare/distribution_function.h>
#include <poincare/binomial_distribution.h>
#include <poincare/geometric_distribution.h>
#include <poincare/normal_distribution.h>
#include <poincare/poisson_distribution.h>
#include <poincare/student_distribution.h>
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

  bool couldCheckParamters;
  bool parametersAreOk;
  int i = numberOfParameters(functionType());

  switch (distributionType()) {
  case DistributionType::Normal:
    couldCheckParamters = NormalDistribution::ExpressionMuAndVarAreOK(
      &parametersAreOk,
      childAtIndex(i++),
      childAtIndex(i++),
      context);
    break;
  case DistributionType::Student:
    couldCheckParamters = StudentDistribution::ExpressionKIsOK(
      &parametersAreOk,
      childAtIndex(i++),
      context);
    break;
  case DistributionType::Binomial:
    couldCheckParamters = BinomialDistribution::ExpressionParametersAreOK(
      &parametersAreOk,
      childAtIndex(i++),
      childAtIndex(i++),
      context);
    break;
  case DistributionType::Poisson:
    couldCheckParamters = PoissonDistribution::ExpressionLambdaIsOK(
      &parametersAreOk,
      childAtIndex(i++),
      context);
    break;
  case DistributionType::Geometric:
    couldCheckParamters = GeometricDistribution::ExpressionPIsOK(
      &parametersAreOk,
      childAtIndex(i++),
      context);
    break;
  }

  if (!couldCheckParamters) {
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
  int i = 0;
  T x, y;
  switch (m_functionType) {
      case FunctionType::CDF:
      case FunctionType::PDF:
      case FunctionType::Inverse:
      {
        Evaluation<T> xEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        x = xEvaluation.toScalar();
        break;
      }
      case FunctionType::CDFRange:
      {
        Evaluation<T> xEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        Evaluation<T> yEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        x = xEvaluation.toScalar();
        y = yEvaluation.toScalar();
        break;
      }
  }
  T result = NAN;
  T castedOne = static_cast<T>(1.0);
  switch (m_distributionType) {
      case DistributionType::Normal:
      {
        Evaluation<T> muEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        Evaluation<T> sigmaEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        const T mu = muEvaluation.toScalar();
        const T sigma = sigmaEvaluation.toScalar();
        switch (m_functionType) {
          case FunctionType::CDF:
            result = NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(x, mu, sigma);
            break;
          case FunctionType::CDFRange:
            result = NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(y, mu, sigma) -  NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(x, mu, sigma);
            break;
          case FunctionType::PDF:
            result = NormalDistribution::EvaluateAtAbscissa(x, mu, sigma);
            break;
          case FunctionType::Inverse:
            result = NormalDistribution::CumulativeDistributiveInverseForProbability(x, mu, sigma);
            break;
          default:
            assert(false);
        }
        break;
      }
      case DistributionType::Student:
      {
        Evaluation<T> lambdaEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        const T lambda = lambdaEvaluation.toScalar();
        switch (m_functionType) {
          case FunctionType::CDF:
            result = StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(x, lambda);
            break;
          case FunctionType::CDFRange:
            result = StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(y, lambda) -  StudentDistribution::CumulativeDistributiveFunctionAtAbscissa(x, lambda);
            break;
          case FunctionType::PDF:
            result = StudentDistribution::EvaluateAtAbscissa(x, lambda);
            break;
          case FunctionType::Inverse:
            result = StudentDistribution::CumulativeDistributiveInverseForProbability(x, lambda);
            break;
          default:
            assert(false);
        }
        break;
      }
      case DistributionType::Binomial:
      {
        Evaluation<T> nEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        Evaluation<T> pEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        const T n = nEvaluation.toScalar();
        const T p = pEvaluation.toScalar();
        switch (m_functionType) {
          case FunctionType::CDF:
            result = BinomialDistribution::CumulativeDistributiveFunctionAtAbscissa(x, n, p);
            break;
          case FunctionType::PDF:
            result = BinomialDistribution::EvaluateAtAbscissa(x, n, p);
            break;
          case FunctionType::Inverse:
            result = BinomialDistribution::CumulativeDistributiveInverseForProbability(x, n, p);
            break;
          default:
            assert(false);
        }
        break;
      }
     case DistributionType::Poisson:
     {
        Evaluation<T> lambdaEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        const T lambda = lambdaEvaluation.toScalar();
        switch (m_functionType) {
          case FunctionType::CDF:
            result = PoissonDistribution::CumulativeDistributiveFunctionAtAbscissa(x, lambda);
            break;
          case FunctionType::PDF:
            result = PoissonDistribution::EvaluateAtAbscissa(x, lambda);
            break;
          default:
            assert(false);
        }
        break;
     }
     case DistributionType::Geometric:
     {
        Evaluation<T> pEvaluation = childAtIndex(i++)->approximate(T(), approximationContext);
        const T p = pEvaluation.toScalar();
        switch (m_functionType) {
          case FunctionType::CDF:
            result = GeometricDistribution::CumulativeDistributiveFunctionAtAbscissa(x, p);
            break;
          case FunctionType::CDFRange:
            result = GeometricDistribution::CumulativeDistributiveFunctionAtAbscissa(y, p) - GeometricDistribution::CumulativeDistributiveFunctionAtAbscissa(x - castedOne, p);
            break;
          case FunctionType::PDF:
            result = GeometricDistribution::EvaluateAtAbscissa(x, p);
            break;
          case FunctionType::Inverse:
            result = GeometricDistribution::CumulativeDistributiveInverseForProbability(x, p);
            break;
          default:
            assert(false);
        }
        break;
     }
  }
  return Complex<T>::Builder(result);
}
}
