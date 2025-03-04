#include "inference.h"

#include <omg/round.h>
#include <omg/unreachable.h>
#include <poincare/k_tree.h>
#include <poincare/src/statistics/domain.h>
#include <poincare/statistics/distribution.h>

#include "poincare/src/layout/k_tree.h"

namespace Poincare::Internal::Inference {

Distribution::Type DistributionType(StatisticType statisticType) {
  switch (statisticType) {
    case StatisticType::T:
    case StatisticType::TPooled:
      return Distribution::Type::Student;
    case StatisticType::Z:
      return Distribution::Type::Normal;
    case StatisticType::Chi2:
      return Distribution::Type::Chi2;
    default:
      OMG::unreachable();
  }
}

Distribution::ParametersArray<double> DistributionParameters(
    StatisticType statisticType, double degreesOfFreedom) {
  Distribution::Type distributionType = DistributionType(statisticType);
  switch (distributionType) {
    case Distribution::Type::Student:
    case Distribution::Type::Chi2:
      return Distribution::ParametersArray<double>({degreesOfFreedom});
    case Distribution::Type::Normal:
      return Distribution::ParametersArray<double>({0., 1.});
    default:
      OMG::unreachable();
  }
}

int NumberOfTestsForMethod(Method method) {
  const TestType testsArray[] = {TestType::OneProportion,  TestType::OneMean,
                                 TestType::TwoProportions, TestType::TwoMeans,
                                 TestType::Chi2,           TestType::Slope};
  int count = 0;
  for (TestType testType : testsArray) {
    if (IsMethodCompatibleWithTest(method, testType)) {
      count++;
    }
  }
  return count;
}

int NumberOfStatisticsForTest(TestType testType) {
  const StatisticType statsArray[] = {StatisticType::T, StatisticType::TPooled,
                                      StatisticType::Z, StatisticType::Chi2};
  int count = 0;
  for (StatisticType statisticType : statsArray) {
    if (IsTestCompatibleWithStatistic(testType, statisticType)) {
      count++;
    }
  }
  return count;
}

bool HasDegreesOfFreedom(Type type) {
  return type.statisticType != StatisticType::Z &&
         (type.statisticType != StatisticType::Chi2 ||
          type.categoricalType != CategoricalType::GoodnessOfFit);
}

double ComputeDegreesOfFreedom(Type type, const ParametersArray parameters) {
  if (!HasDegreesOfFreedom(type)) {
    return NAN;
  }
  switch (type.testType) {
    case TestType::OneMean:
      return parameters[Params::OneMean::N] - 1;
    case TestType::TwoMeans: {
      double n1 = parameters[Params::TwoMeans::N1];
      double n2 = parameters[Params::TwoMeans::N2];
      if (type.statisticType == StatisticType::TPooled) {
        return n1 + n2 - 2;
      }
      double s1 = parameters[Params::TwoMeans::S1];
      double s2 = parameters[Params::TwoMeans::S2];
      double v1 = std::pow(s1, 2.) / n1;
      double v2 = std::pow(s2, 2.) / n2;
      return std::pow(v1 + v2, 2.) /
             (std::pow(v1, 2.) / (n1 - 1.) + std::pow(v2, 2.) / (n2 - 1.));
    }
    case TestType::Slope:
      return parameters[Params::Slope::N] - 2;
    case TestType::Chi2:
      // TODO: Implement
    default:
      // OneProportion, TwoProportions
      OMG::unreachable();
  }
}

bool IsH0Valid(TestType testType, double h0) {
  switch (testType) {
    case TestType::OneProportion:
      return h0 >= 0 && h0 <= 1;
    case TestType::TwoProportions:
      return h0 >= -1 && h0 <= 1;
    case TestType::OneMean:
    case TestType::TwoMeans:
    case TestType::Slope:
      return true;
    default:
      // Chi2 doesn't have a hypothesis
      OMG::unreachable();
  }
}

SignificanceTestResults ComputeSignificanceTest(Type type,
                                                Hypothesis hypothesis,
                                                const ParametersArray params) {
  Estimates estimates = ComputeTestEstimates(type, params);
  double degreesOfFreedom = ComputeDegreesOfFreedom(type, params);
  double criticalValue =
      ComputeTestCriticalValue(type, hypothesis.m_h0, params);
  double pValue = ComputePValue(type, hypothesis.m_alternative, criticalValue,
                                degreesOfFreedom);
  return SignificanceTestResults{.estimates = estimates,
                                 .criticalValue = criticalValue,
                                 .pValue = pValue,
                                 .degreesOfFreedom = degreesOfFreedom};
}

Estimates ComputeTestEstimates(TestType testType,
                               const ParametersArray parameters) {
  Estimates estimates{NAN, NAN, NAN};
  switch (testType) {
    case TestType::OneProportion: {
      double x = parameters[Params::OneProportion::X];
      double n = parameters[Params::OneProportion::N];
      estimates[EstimatesOrder::OneProportion::P] = x / n;
      break;
    }
    case TestType::TwoProportions: {
      double x1 = parameters[Params::TwoProportions::X1];
      double n1 = parameters[Params::TwoProportions::N1];
      double x2 = parameters[Params::TwoProportions::X2];
      double n2 = parameters[Params::TwoProportions::N2];
      estimates[EstimatesOrder::TwoProportions::P1] = x1 / n1;
      estimates[EstimatesOrder::TwoProportions::P2] = x2 / n2;
      estimates[EstimatesOrder::TwoProportions::Pooled] = (x1 + x2) / (n1 + n2);
      break;
    }
    default:
      break;
  }
  return estimates;
}

double ComputeTestCriticalValue(Type type, double h0,
                                const ParametersArray parameters) {
  switch (type.testType) {
    case TestType::OneProportion: {
      double x = parameters[Params::OneProportion::X];
      double n = parameters[Params::OneProportion::N];
      double p = x / n;
      return (p - h0) / std::sqrt(h0 * (1 - h0) / n);
    }
    case TestType::TwoProportions: {
      double x1 = parameters[Params::TwoProportions::X1];
      double n1 = parameters[Params::TwoProportions::N1];
      double x2 = parameters[Params::TwoProportions::X2];
      double n2 = parameters[Params::TwoProportions::N2];
      double p1 = x1 / n1;
      double p2 = x2 / n2;
      double p = (x1 + x2) / (n1 + n2);
      return (p1 - p2 - h0) / std::sqrt(p * (1 - p) * (1. / n1 + 1. / n2));
    }
    case TestType::OneMean: {
      double x = parameters[Params::OneMean::X];
      double s = parameters[Params::OneMean::S];
      double n = parameters[Params::OneMean::N];
      return (x - h0) / (s / std::sqrt(n));
    }
    case TestType::TwoMeans: {
      double x1 = parameters[Params::TwoMeans::X1];
      double x2 = parameters[Params::TwoMeans::X2];
      return ((x1 - x2) - h0) / ComputeStandardError(type, parameters);
    }
    case TestType::Slope:
      return (parameters[Params::Slope::B] - h0) /
             ComputeStandardError(type, parameters);
    case TestType::Chi2:
      // TODO
      assert(false);
      return NAN;
    default:
      OMG::unreachable();
  }
}

double ComputePValue(Type type, ComparisonJunior::Operator haOperator,
                     double criticalValue, double degreesOfFreedom) {
  Distribution::Type distrib = DistributionType(type);
  Distribution::ParametersArray<double> distribParams =
      DistributionParameters(type, degreesOfFreedom);

  switch (haOperator) {
    case ComparisonJunior::Operator::Inferior:
      return Distribution::CumulativeDistributiveFunctionAtAbscissa(
          distrib, criticalValue, distribParams);
    case ComparisonJunior::Operator::Superior:
      return 1.0 - Distribution::CumulativeDistributiveFunctionAtAbscissa(
                       distrib, criticalValue, distribParams);
    case ComparisonJunior::Operator::NotEqual:
      return 2.0 * Distribution::CumulativeDistributiveFunctionAtAbscissa(
                       distrib, -std::fabs(criticalValue), distribParams);
    default:
      OMG::unreachable();
  }
}

ConfidenceIntervalResults ComputeConfidenceInterval(
    Type type, double threshold, const ParametersArray params) {
  double degreesOfFreedom = ComputeDegreesOfFreedom(type, params);
  double estimate = ComputeIntervalEstimate(type, params);
  double standardError = ComputeStandardError(type, params);
  double zCritical =
      ComputeIntervalCriticalValue(type, threshold, degreesOfFreedom);
  double marginOfError = ComputeMarginOfError(zCritical, standardError);
  return ConfidenceIntervalResults{.estimate = estimate,
                                   .zCritical = zCritical,
                                   .standardError = standardError,
                                   .marginOfError = marginOfError,
                                   .degreesOfFreedom = degreesOfFreedom};
}

double ComputeIntervalCriticalValue(Type type, double threshold,
                                    double degreesOfFreedom) {
  Distribution::Type distrib = DistributionType(type);
  Distribution::ParametersArray<double> distribParams =
      DistributionParameters(type, degreesOfFreedom);
  double proba = 0.5 + threshold / 2;
  return Distribution::CumulativeDistributiveInverseForProbability(
      distrib, proba, distribParams);
}

// Return -1 if the estimate is not directly in the parameters
int IndexOfParameterToUseAsIntervalEstimate(TestType testType) {
  switch (testType) {
    case TestType::OneMean:
      return Params::OneMean::X;
    case TestType::Slope:
      return Params::Slope::B;
    case TestType::Chi2:
      // TODO
      assert(false);
    default:
      OMG::unreachable();
  }
}

bool ShowIntervalEstimate(TestType testType) {
  return IndexOfParameterToUseAsIntervalEstimate(testType) < 0;
}

double ComputeIntervalEstimate(TestType testType,
                               const ParametersArray parameters) {
  int index = IndexOfParameterToUseAsIntervalEstimate(testType);
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
    case TestType::Chi2:
      // TODO
      assert(false);
      return NAN;
    default:
      OMG::unreachable();
  }
}

double ComputeStandardError(Type type, const ParametersArray parameters) {
  switch (type.testType) {
    case TestType::OneProportion: {
      double estimate = ComputeIntervalEstimate(type, parameters);
      double n = parameters[Params::OneProportion::N];
      return OMG::LaxToZero(std::sqrt(estimate * (1 - estimate) / n));
    }
    case TestType::TwoProportions: {
      double estimate = ComputeIntervalEstimate(type, parameters);
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
    case TestType::TwoMeans: {
      double s1 = parameters[Params::TwoMeans::S1];
      double n1 = parameters[Params::TwoMeans::N1];
      double s2 = parameters[Params::TwoMeans::S2];
      double n2 = parameters[Params::TwoMeans::N2];
      if (type.statisticType == StatisticType::TPooled) {
        double sp = std::sqrt(((n1 - 1.) * s1 * s1 + (n2 - 1.) * s2 * s2) /
                              (n1 + n2 - 2.));
        return OMG::LaxToZero(sp * std::sqrt(1. / n1 + 1. / n2));
      }
      return OMG::LaxToZero(std::sqrt((s1 * s1 / n1 + s2 * s2 / n2)));
    }
    case TestType::Slope:
      return parameters[Params::Slope::SE];
    case TestType::Chi2:
      // TODO
      assert(false);
      return NAN;
    default:
      OMG::unreachable();
  }
}

constexpr static KTree KXBar =
    KCombinedCodePointL<'x', UCodePointCombiningOverline>();
constexpr static KTree KPHat =
    KCombinedCodePointL<'p', UCodePointCombiningCircumflex>();

constexpr static const Tree* k_x = "x"_l;
constexpr static const Tree* k_xBar = KRackL(KXBar);
constexpr static const Tree* k_n = "n"_l;
constexpr static const Tree* k_s = "s"_l;
constexpr static const Tree* k_sigma = "σ"_l;
constexpr static const Tree* k_b = "b"_l;
constexpr static const Tree* k_se = "SE"_l;
constexpr static const Tree* k_x1 = "x"_l ^ KSubscriptL("1"_l);
constexpr static const Tree* k_x1Bar = KRackL(KXBar) ^ KSubscriptL("1"_l);
constexpr static const Tree* k_n1 = "n"_l ^ KSubscriptL("1"_l);
constexpr static const Tree* k_s1 = "s"_l ^ KSubscriptL("1"_l);
constexpr static const Tree* k_sigma1 = "σ"_l ^ KSubscriptL("1"_l);
constexpr static const Tree* k_x2 = "x"_l ^ KSubscriptL("2"_l);
constexpr static const Tree* k_x2Bar = KRackL(KXBar) ^ KSubscriptL("2"_l);
constexpr static const Tree* k_n2 = "n"_l ^ KSubscriptL("2"_l);
constexpr static const Tree* k_s2 = "s"_l ^ KSubscriptL("2"_l);
constexpr static const Tree* k_sigma2 = "σ"_l ^ KSubscriptL("2"_l);

constexpr static const Tree* k_oneProplayouts[] = {k_x, k_n};
constexpr static const Tree* k_twoPropslayouts[] = {k_x1, k_n1, k_x2, k_n2};
constexpr static const Tree* k_oneMeanTTestlayouts[] = {k_xBar, k_s, k_n};
constexpr static const Tree* k_oneMeanZTestlayouts[] = {k_xBar, k_sigma, k_n};
constexpr static const Tree* k_twoMeansTTestLayouts[] = {k_x1Bar, k_s1, k_n1,
                                                         k_x2Bar, k_s2, k_n2};
constexpr static const Tree* k_twoMeansZTestLayouts[] = {
    k_x1Bar, k_sigma1, k_n1, k_x2Bar, k_sigma2, k_n2};
constexpr static const Tree* k_slopeLayouts[] = {k_n, k_b, k_se};

Poincare::Layout ParameterLayout(Type type, int index) {
  assert(index >= 0 && index < NumberOfParameters(type));
  switch (type.testType) {
    case TestType::OneProportion:
      return k_oneProplayouts[index];
    case TestType::TwoProportions:
      return k_twoPropslayouts[index];
    case TestType::OneMean:
      return type.statisticType == StatisticType::Z
                 ? k_oneMeanZTestlayouts[index]
                 : k_oneMeanTTestlayouts[index];
    case TestType::TwoMeans:
      return type.statisticType == StatisticType::Z
                 ? k_twoMeansZTestLayouts[index]
                 : k_twoMeansTTestLayouts[index];
    case TestType::Slope:
      return k_slopeLayouts[index];
    case TestType::Chi2:
      // TODO
      assert(false);
    default:
      OMG::unreachable();
  }
}

const char* HypothesisSymbol(TestType testType) {
  switch (testType) {
    case TestType::OneProportion:
      return "p";
    case TestType::TwoProportions:
      return "p1-p2";
    case TestType::OneMean:
      return "μ";
    case TestType::TwoMeans:
      return "μ1-μ2";
    case TestType::Slope:
      return "β";
    default:
      // Chi2 doesn't have a hypothesis
      OMG::unreachable();
  }
}

Poincare::Layout HypothesisLayout(TestType testType) {
  if (testType == TestType::TwoMeans) {
    return "p"_l ^ KSubscriptL("1"_l) ^ "-"_l ^ "p"_l ^ KSubscriptL("2"_l);
  }
  if (testType == TestType::TwoProportions) {
    return "μ"_l ^ KSubscriptL("1"_l) ^ "-"_l ^ "μ"_l ^ KSubscriptL("2"_l);
  }
  return Layout::String(HypothesisSymbol(testType));
}

const char* CriticalValueSymbol(StatisticType statisticType) {
  Distribution::Type distributionType = DistributionType(statisticType);
  switch (distributionType) {
    case Distribution::Type::Student:
      return "t";
    case Distribution::Type::Normal:
      return "z";
    case Distribution::Type::Chi2:
      return "χ2";
    default:
      OMG::unreachable();
  }
}

Poincare::Layout CriticalValueLayout(Method method,
                                     StatisticType statisticType) {
  if (method == Method::ConfidenceInterval) {
    return Layout::Create(
        KA ^ KSuperscriptL("*"_l),
        {.KA = CriticalValueLayout(Method::SignificanceTest, statisticType)
                   .tree()});
  }
  if (statisticType == StatisticType::Chi2) {
    return Layout("χ"_l ^ KSuperscriptL("2"_l));
  }
  return Layout::String(CriticalValueSymbol(statisticType));
}

Poincare::Layout TestEstimateLayout(TestType testType, int index) {
  switch (testType) {
    case TestType::OneProportion:
      return KRackL(KPHat);
    case TestType::TwoProportions:
      switch (index) {
        case EstimatesOrder::TwoProportions::P1:
          return KPHat ^ KSubscriptL("1"_l);
        case EstimatesOrder::TwoProportions::P2:
          return KPHat ^ KSubscriptL("2"_l);
        case EstimatesOrder::TwoProportions::Pooled:
          return KRackL(KPHat);
        default:
          OMG::unreachable();
      }
    default:
      OMG::unreachable();
  }
}

const char* IntervalEstimateSymbol(TestType testType) {
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

Poincare::Layout IntervalEstimateLayout(Type type) {
  int paramIndex = IndexOfParameterToUseAsIntervalEstimate(type);
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
    case TestType::Chi2:
      // TODO
      assert(false);
    default:
      OMG::unreachable();
  }
}

double DefaultSignificanceTestParameterAtIndex(Type type, int index) {
  ParametersArray defaultParameters;
  switch (type.testType) {
    case TestType::OneProportion:
      defaultParameters[Params::OneProportion::X] = 47.;
      defaultParameters[Params::OneProportion::N] = 500.;
      break;
    case TestType::TwoProportions:
      defaultParameters[Params::TwoProportions::X1] = 19.;
      defaultParameters[Params::TwoProportions::N1] = 80.;
      defaultParameters[Params::TwoProportions::X2] = 26.;
      defaultParameters[Params::TwoProportions::N2] = 150.;
      break;
    case TestType::OneMean:
      defaultParameters[Params::OneMean::X] = 47.9;
      defaultParameters[Params::OneMean::S] = 2.81;
      defaultParameters[Params::OneMean::N] = 12.;
      break;
    case TestType::TwoMeans:
      defaultParameters[Params::TwoMeans::X1] = 5.;
      defaultParameters[Params::TwoMeans::S1] = 8.743;
      defaultParameters[Params::TwoMeans::N1] = 10.;
      defaultParameters[Params::TwoMeans::X2] = -0.273;
      defaultParameters[Params::TwoMeans::S2] = 5.901;
      defaultParameters[Params::TwoMeans::N2] = 11.;
      break;
    case TestType::Slope:
      defaultParameters[Params::Slope::B] = 0.5;
      defaultParameters[Params::Slope::SE] = 0.1;
      defaultParameters[Params::Slope::N] = 10.;
      break;
    case TestType::Chi2:
      // TODO
      assert(false);
    default:
      OMG::unreachable();
  }
  return defaultParameters[index];
}

double DefaultConfidenceIntervalParameterAtIndex(Type type, int index) {
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
    case TestType::Chi2:
      // TODO
      assert(false);
    default:
      OMG::unreachable();
  }
  return defaultParameters[index];
}

double DefaultParameterAtIndex(Method method, Type type, int index) {
  switch (method) {
    case Method::SignificanceTest:
      return DefaultSignificanceTestParameterAtIndex(type, index);
    case Method::ConfidenceInterval:
      return DefaultConfidenceIntervalParameterAtIndex(type, index);
    default:
      OMG::unreachable();
  }
}

Hypothesis DefaultHyphothesis(TestType testType) {
  switch (testType) {
    case TestType::OneProportion:
      return Hypothesis{0.08, ComparisonJunior::Operator::Superior};
    case TestType::TwoProportions:
      return Hypothesis{0., ComparisonJunior::Operator::NotEqual};
    case TestType::OneMean:
      return Hypothesis{50., ComparisonJunior::Operator::Inferior};
    case TestType::TwoMeans:
    case TestType::Slope:
      return Hypothesis{0., ComparisonJunior::Operator::Superior};
    default:
      // Chi2 doesn't have a hypothesis
      OMG::unreachable();
  }
}

bool IsThresholdValid(double threshold) {
  /* A threshold of 1.0 does not make sense mathematically speaking and can
   * cause some results to be infinite. */
  return Domain::ContainsFloat(threshold, Domain::Type::ZeroToOneExcluded);
}

bool IsParameterValidAtIndex(Type type, double p, int index) {
  switch (type.testType) {
    case TestType::OneProportion:
    case TestType::TwoProportions: {
      int onePropIndex = index % NumberOfParameters(TestType::OneProportion);
      switch (onePropIndex) {
        case Params::OneProportion::X:
          return Domain::ContainsFloat(p, Domain::Type::N);
        case Params::OneProportion::N:
          return Domain::ContainsFloat(p, Domain::Type::NStar);
        default:
          OMG::unreachable();
      }
    }
    case TestType::OneMean:
    case TestType::TwoMeans: {
      int oneMeanIndex = index % NumberOfParameters(TestType::OneMean);
      switch (oneMeanIndex) {
        case Params::OneMean::X:
          return true;
        case Params::OneMean::S:
          return Domain::ContainsFloat(p, Domain::Type::RPlus) &&
                 // Accept 0 for TwoMeans, as one of the two can be 0
                 (type.testType == TestType::TwoMeans || p > 0.);
        case Params::OneMean::N:
          return Domain::ContainsFloat(p, Domain::Type::NStar) &&
                 (type.statisticType == StatisticType::Z || p >= 2.);
        default:
          OMG::unreachable();
      }
    }
    case TestType::Slope:
      switch (index) {
        case Params::Slope::N:
          return Domain::ContainsFloat(p, Domain::Type::NStar) && p >= 3.;
        case Params::Slope::B:
          return true;
        case Params::Slope::SE:
          return Domain::ContainsFloat(p, Domain::Type::RPlus);
        default:
          OMG::unreachable();
      }
    case TestType::Chi2:
      // TODO
      assert(false);
    default:
      OMG::unreachable();
  }
}

bool AreParametersValid(Type type, const ParametersArray& parameters) {
  for (int i = 0; i < NumberOfParameters(type); i++) {
    if (!IsParameterValidAtIndex(type, parameters[i], i)) {
      return false;
    }
  }
  // TODO: Apply this logic to Distributions (check combined conditions after)
  switch (type.testType) {
    case TestType::OneProportion:
      return parameters[Params::OneProportion::X] <=
             parameters[Params::OneProportion::N];
    case TestType::TwoProportions:
      return parameters[Params::TwoProportions::X1] <=
                 parameters[Params::TwoProportions::N1] &&
             parameters[Params::TwoProportions::X2] <=
                 parameters[Params::TwoProportions::N2] &&
             (parameters[Params::TwoProportions::X1] > 0. ||
              parameters[Params::TwoProportions::X2] > 0.);
    case TestType::TwoMeans:
      return (parameters[Params::TwoMeans::S1] > 0. ||
              parameters[Params::TwoMeans::S2] > 0.) &&
             (type.statisticType == StatisticType::Z ||
              ComputeDegreesOfFreedom(type, parameters) > 0.);
    default:
      return true;
  }
}

}  // namespace Poincare::Internal::Inference
