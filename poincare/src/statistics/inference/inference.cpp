#include "inference.h"

#include <omg/round.h>
#include <omg/unreachable.h>
#include <poincare/k_tree.h>
#include <poincare/src/statistics/domain.h>
#include <poincare/statistics/distribution.h>

#include "poincare/src/layout/k_tree.h"

namespace Poincare::Internal::Inference {

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

bool HasDegreesOfFreedom(Type type) {
  return type.statisticType != StatisticType::Z;
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
    default:
      // OneProportion, TwoProportions
      // Chi2 is handled separately
      OMG::unreachable();
  }
}

constexpr static KTree KXBar =
    KCombinedCodePointL<'x', UCodePointCombiningOverline>();

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

bool IsThresholdValid(double threshold) {
  /* A threshold of 1.0 does not make sense mathematically speaking and can
   * cause some results to be infinite. */
  return Domain::ContainsFloat(threshold, Domain::Type::ZeroToOneExcluded);
}

double TwoMeansStandardError(StatisticType statisticType,
                             const ParametersArray parameters) {
  double s1 = parameters[Params::TwoMeans::S1];
  double n1 = parameters[Params::TwoMeans::N1];
  double s2 = parameters[Params::TwoMeans::S2];
  double n2 = parameters[Params::TwoMeans::N2];
  if (statisticType == StatisticType::TPooled) {
    double sp =
        std::sqrt(((n1 - 1.) * s1 * s1 + (n2 - 1.) * s2 * s2) / (n1 + n2 - 2.));
    return OMG::LaxToZero(sp * std::sqrt(1. / n1 + 1. / n2));
  }
  return OMG::LaxToZero(std::sqrt((s1 * s1 / n1 + s2 * s2 / n2)));
}

}  // namespace Poincare::Internal::Inference
