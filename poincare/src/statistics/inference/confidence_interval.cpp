#include "confidence_interval.h"

#include <omg/round.h>
#include <omg/unreachable.h>
#include <poincare/k_tree.h>
#include <poincare/src/statistics/domain.h>
#include <poincare/statistics/distribution.h>

#include "poincare/src/layout/k_tree.h"
#include "significance_test.h"

namespace Poincare::Internal::Inference::ConfidenceInterval {

Results Compute(Type type, double threshold, const ParametersArray params) {
  assert(IsTypeCompatibleWithConfidenceInterval(type));
  double degreesOfFreedom = ComputeDegreesOfFreedom(type, params);
  double estimate = ComputeEstimate(type, params);
  double standardError = ComputeStandardError(type, params);
  double zCritical = ComputeCriticalValue(type, threshold, degreesOfFreedom);
  double marginOfError = ComputeMarginOfError(zCritical, standardError);
  return Results{.estimate = estimate,
                 .zCritical = zCritical,
                 .standardError = standardError,
                 .marginOfError = marginOfError,
                 .degreesOfFreedom = degreesOfFreedom};
}

double ComputeCriticalValue(Type type, double threshold,
                            double degreesOfFreedom) {
  Distribution::Type distrib = DistributionType(type);
  Distribution::ParametersArray<double> distribParams =
      DistributionParameters(type, degreesOfFreedom);
  double proba = 0.5 + threshold / 2;
  return Distribution::CumulativeDistributiveInverseForProbability(
      distrib, proba, distribParams);
}

// Return -1 if the estimate is not directly in the parameters
int IndexOfParameterToUseAsEstimate(TestType testType) {
  switch (testType) {
    case TestType::OneMean:
      return Params::OneMean::X;
    case TestType::Slope:
      return Params::Slope::B;
    default:
      return -1;
  }
}

bool ShowEstimate(TestType testType) {
  return IndexOfParameterToUseAsEstimate(testType) < 0;
}

double ComputeEstimate(TestType testType, const ParametersArray parameters) {
  int index = IndexOfParameterToUseAsEstimate(testType);
  if (index >= 0) {
    return parameters[index];
  }
  switch (testType) {
    case TestType::OneProportion:
      return parameters[Params::OneProportion::X] /
             parameters[Params::OneProportion::N];
    case TestType::TwoProportions: {
      double x1 = parameters[Params::TwoProportions::X1];
      double n1 = parameters[Params::TwoProportions::N1];
      double x2 = parameters[Params::TwoProportions::X2];
      double n2 = parameters[Params::TwoProportions::N2];
      return x1 / n1 - x2 / n2;
    }
    case TestType::TwoMeans:
      return parameters[Params::TwoMeans::X1] -
             parameters[Params::TwoMeans::X2];
    default:
      // Chi2 doesn't have an interval estimate
      OMG::unreachable();
  }
}

double ComputeStandardError(Type type, const ParametersArray parameters) {
  switch (type.testType) {
    case TestType::OneProportion: {
      double estimate = ComputeEstimate(type, parameters);
      double n = parameters[Params::OneProportion::N];
      return OMG::LaxToZero(std::sqrt(estimate * (1 - estimate) / n));
    }
    case TestType::TwoProportions: {
      double estimate = ComputeEstimate(type, parameters);
      double n1 = parameters[Params::TwoProportions::N1];
      double n2 = parameters[Params::TwoProportions::N2];
      return OMG::LaxToZero(
          std::sqrt(estimate * (1 - estimate) * (1. / n1 + 1. / n2)));
    }
    case TestType::OneMean: {
      double s = parameters[Params::OneMean::S];
      double n = parameters[Params::OneMean::N];
      return s / std::sqrt(n);
    }
    case TestType::TwoMeans:
      return TwoMeansStandardError(type, parameters);
    case TestType::Slope:
      return parameters[Params::Slope::SE];
    default:
      // Chi2 doesn't have an interval estimate
      OMG::unreachable();
  }
}

constexpr static KTree KXBar =
    KCombinedCodePointL<'x', UCodePointCombiningOverline>();
constexpr static KTree KPHat =
    KCombinedCodePointL<'p', UCodePointCombiningCircumflex>();

Poincare::Layout CriticalValueLayout(StatisticType statisticType) {
  return Layout::Create(
      KA ^ KSuperscriptL("*"_l),
      {.KA = SignificanceTest::CriticalValueLayout(statisticType).tree()});
}

const char* EstimateSymbol(TestType testType) {
  switch (testType) {
    case TestType::OneProportion:
      return "p̂";
    case TestType::TwoProportions:
      return "p̂1-p̂2";
    case TestType::OneMean:
      return "x̅";
    case TestType::TwoMeans:
      return "x̅1-x̅2";
    case TestType::Slope:
      return "b";
    default:
      // Chi2 doesn't have an interval estimate
      OMG::unreachable();
  }
}

Poincare::Layout EstimateLayout(Type type) {
  int paramIndex = IndexOfParameterToUseAsEstimate(type);
  if (paramIndex >= 0) {
    return ParameterLayout(type, paramIndex);
  }
  switch (type.testType) {
    case TestType::OneProportion:
      return KRackL(KPHat);
    case TestType::TwoProportions:
      return KPHat ^ KSubscriptL("1"_l) ^ "-"_l ^ KPHat ^ KSubscriptL("2"_l);
    case TestType::TwoMeans:
      return KXBar ^ KSubscriptL("1"_l) ^ "-"_l ^ KXBar ^ KSubscriptL("2"_l);
    default:
      // Chi2 doesn't have an interval estimate
      OMG::unreachable();
  }
}

double DefaultParameterAtIndex(Type type, int index) {
  ParametersArray defaultParameters;
  switch (type.testType) {
    case TestType::OneProportion:
      defaultParameters[Params::OneProportion::X] = 107.;
      defaultParameters[Params::OneProportion::N] = 251.;
      break;
    case TestType::TwoProportions:
      defaultParameters[Params::TwoProportions::X1] = 639.;
      defaultParameters[Params::TwoProportions::N1] = 799.;
      defaultParameters[Params::TwoProportions::X2] = 1555.;
      defaultParameters[Params::TwoProportions::N2] = 2253.;
      break;
    case TestType::OneMean: {
      switch (type.statisticType) {
        case StatisticType::T:
          defaultParameters[Params::OneMean::X] = 1.2675;
          defaultParameters[Params::OneMean::S] = 0.3332;
          defaultParameters[Params::OneMean::N] = 40.;
          break;
        case StatisticType::Z:
          defaultParameters[Params::OneMean::X] = 240.8;
          defaultParameters[Params::OneMean::S] = 16.;
          defaultParameters[Params::OneMean::N] = 20.;
          break;
        default:
          OMG::unreachable();
      }
      break;
    }
    case TestType::TwoMeans:
      defaultParameters[Params::TwoMeans::X1] = 23.7;
      defaultParameters[Params::TwoMeans::S1] = 17.5;
      defaultParameters[Params::TwoMeans::N1] = 30.;
      defaultParameters[Params::TwoMeans::X2] = 34.53;
      defaultParameters[Params::TwoMeans::S2] = 14.26;
      defaultParameters[Params::TwoMeans::N2] = 30.;
      break;
    case TestType::Slope:
      // TODO: These values were invented. No spec yet
      defaultParameters[Params::Slope::N] = 10.;
      defaultParameters[Params::Slope::B] = 0.5;
      defaultParameters[Params::Slope::SE] = 0.1;
      break;
    default:
      // Chi2 doesn't have an interval estimate
      OMG::unreachable();
  }
  return defaultParameters[index];
}

}  // namespace Poincare::Internal::Inference::ConfidenceInterval
