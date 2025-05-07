#include "significance_test.h"

#include <omg/round.h>
#include <omg/unreachable.h>
#include <poincare/k_tree.h>
#include <poincare/src/statistics/domain.h>
#include <poincare/statistics/distribution.h>

#include "poincare/src/layout/k_tree.h"

namespace Poincare::Internal::Inference::SignificanceTest {

bool IsH0Valid(TestType testType, double h0) {
  switch (testType) {
    case TestType::OneProportion:
      return h0 >= 0. && h0 <= 1.;
    case TestType::TwoProportions:
      return h0 >= -1. && h0 <= 1.;
    case TestType::OneMean:
    case TestType::TwoMeans:
    case TestType::Slope:
    case TestType::Chi2:
      return true;
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
      // Chi2 doesn't have a visible hypothesis
      OMG::unreachable();
  }
}

Poincare::Layout HypothesisLayout(TestType testType) {
  if (testType == TestType::TwoProportions) {
    return "p"_l ^ KSubscriptL("1"_l) ^ "-"_l ^ "p"_l ^ KSubscriptL("2"_l);
  }
  if (testType == TestType::TwoMeans) {
    return "μ"_l ^ KSubscriptL("1"_l) ^ "-"_l ^ "μ"_l ^ KSubscriptL("2"_l);
  }
  return Layout::String(HypothesisSymbol(testType));
}

Results Compute(Type type, Hypothesis hypothesis,
                const ParametersArray params) {
  assert(type.testType != TestType::Chi2);
  Estimates estimates = ComputeEstimates(type, params);
  double degreesOfFreedom = ComputeDegreesOfFreedom(type, params);
  double criticalValue = ComputeCriticalValue(type, hypothesis.m_h0, params);
  double pValue = ComputePValue(type, hypothesis.m_alternative, criticalValue,
                                degreesOfFreedom);
  return Results{.estimates = estimates,
                 .criticalValue = criticalValue,
                 .pValue = pValue,
                 .degreesOfFreedom = degreesOfFreedom};
}

Estimates ComputeEstimates(TestType testType,
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

double ComputeCriticalValue(Type type, double h0,
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
      return (x1 - x2 - h0) / TwoMeansStandardError(type, parameters);
    }
    case TestType::Slope:
      return (parameters[Params::Slope::B] - h0) /
             parameters[Params::Slope::SE];
    default:
      // Chi2 is special
      OMG::unreachable();
  }
}

double ComputePValue(StatisticType statisticType,
                     ComparisonJunior::Operator haOperator,
                     double criticalValue, double degreesOfFreedom) {
  Distribution::Type distrib = DistributionType(statisticType);
  Distribution::ParametersArray<double> distribParams =
      DistributionParameters(statisticType, degreesOfFreedom);

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

constexpr static KTree KPHat =
    KCombinedCodePointL<'p', UCodePointCombiningCircumflex>();

Poincare::Layout CriticalValueLayout(StatisticType statisticType) {
  if (statisticType == StatisticType::Chi2) {
    return Layout("χ"_l ^ KSuperscriptL("2"_l));
  }
  return Layout::String(CriticalValueSymbol(statisticType));
}

Poincare::Layout EstimateLayoutAtIndex(TestType testType, int index) {
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

Hypothesis DefaultHypothesis(TestType testType) {
  switch (testType) {
    case TestType::OneProportion:
      return Hypothesis{0.08, ComparisonJunior::Operator::Superior};
    case TestType::TwoProportions:
      return Hypothesis{0., ComparisonJunior::Operator::NotEqual};
    case TestType::OneMean:
      return Hypothesis{50., ComparisonJunior::Operator::Inferior};
    case TestType::TwoMeans:
    case TestType::Slope:
    case TestType::Chi2:
      return Hypothesis{0., ComparisonJunior::Operator::Superior};
    default:
      OMG::unreachable();
  }
}

double DefaultParameterAtIndex(Type type, int index) {
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
    default:
      // Chi2 has no default parameters
      OMG::unreachable();
  }
  return defaultParameters[index];
}

}  // namespace Poincare::Internal::Inference::SignificanceTest
