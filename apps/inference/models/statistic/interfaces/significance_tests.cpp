#include "significance_tests.h"
#include <inference/text_helpers.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/vertical_offset_layout.h>
#include <inference/models/statistic/one_mean_t_interval.h>
#include <inference/models/statistic/one_mean_t_test.h>
#include <inference/models/statistic/one_mean_z_interval.h>
#include <inference/models/statistic/one_mean_z_test.h>
#include <inference/models/statistic/pooled_two_means_t_interval.h>
#include <inference/models/statistic/pooled_two_means_t_test.h>
#include <inference/models/statistic/two_means_t_interval.h>
#include <inference/models/statistic/two_means_t_test.h>
#include <inference/models/statistic/two_means_z_interval.h>
#include <inference/models/statistic/two_means_z_test.h>

using namespace Poincare;

namespace Inference {

/* Test */

double SignificanceTest::ComputePValue(Test * t) {
  double z = t->m_testCriticalValue;
  // Compute probability of obtaining a more extreme result
  switch (t->m_hypothesisParams.comparisonOperator()) {
    case HypothesisParams::ComparisonOperator::Higher:
      return 1.0 - t->cumulativeDistributiveFunctionAtAbscissa(z);
    case HypothesisParams::ComparisonOperator::Lower:
      return t->cumulativeDistributiveFunctionAtAbscissa(z);
    case HypothesisParams::ComparisonOperator::Different:
      return 2.0 * t->cumulativeDistributiveFunctionAtAbscissa(-std::fabs(z));
    default:
      assert(false);
      return -1.0;
  }
}

bool SignificanceTest::ValidThreshold(double p) {
  return p >= 0.0 && p <= 1.0;
}

bool SignificanceTest::InitializeDistribution(Statistic * statistic, DistributionType type) {
  if (type == statistic->distributionType()) {
    return false;
  }
  return true;
}

/* OneMean */

bool OneMean::TestInitializeDistribution(Statistic * statistic, DistributionType distributionType) {
  if (!SignificanceTest::InitializeDistribution(statistic, distributionType)) {
    return false;
  }
  statistic->~Statistic();
  switch (distributionType) {
    case DistributionType::T:
      new (statistic) OneMeanTTest();
      break;
    case DistributionType::Z:
      new (statistic) OneMeanZTest();
      break;
    default:
      assert(false);
      break;
  }
  statistic->initParameters();
  return true;
}

bool OneMean::IntervalInitializeDistribution(Statistic * statistic, DistributionType distributionType) {
  if (!SignificanceTest::InitializeDistribution(statistic, distributionType)) {
    return false;
  }
  statistic->~Statistic();
  switch (distributionType) {
    case DistributionType::T:
      new (statistic) OneMeanTInterval();
      break;
    case DistributionType::Z:
      new (statistic) OneMeanZInterval();
      break;
    default:
      assert(false);
      break;
  }
  statistic->initParameters();
  return true;
}

void OneMean::InitTestParameters(Test * t) {
  double * params = t->parametersArray();
  params[ParamsOrder::x] = 47.9;
  t->parametersArray()[ParamsOrder::n] = 12;
  t->parametersArray()[ParamsOrder::s] = 2.81;
  t->m_hypothesisParams.setFirstParam(50);
  t->m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Lower);
}

void OneMean::InitTIntervalParameters(Interval * i) {
  i->parametersArray()[ParamsOrder::x] = 1.2675;
  i->parametersArray()[ParamsOrder::n] = 40;
  i->parametersArray()[ParamsOrder::s] = 0.3332;
}

void OneMean::InitZIntervalParameters(Interval * i) {
  i->parametersArray()[ParamsOrder::x] = 240.80;
  i->parametersArray()[ParamsOrder::n] = 16;
  i->parametersArray()[ParamsOrder::s] = 20;
}


bool OneMean::ZAuthorizedParameterAtIndex(int i, double p) {
  switch (i) {
    case ParamsOrder::n:
      return p >= 1.0;
    default:
      return TAuthorizedParameterAtIndex(i, p);
  }
}

bool OneMean::TAuthorizedParameterAtIndex(int i, double p) {
  if (i == NumberOfParameters()) {
    return ValidThreshold(p);
  }
  switch (i) {
    case ParamsOrder::n:
      return p >= 2.0;
    case ParamsOrder::x:
      return true; // Mean values can be negative
    case ParamsOrder::s:
      return p > 0.0;
    default:
      assert(false);
      return false;
  }
}

double OneMean::ProcessParamaterForIndex(double p, int index) {
  if (index == ParamsOrder::n) {
    p = std::round(p);
    assert(p > 1.0);
  }
  return p;
}

ParameterRepresentation OneMean::ZParameterRepresentationAtIndex(int i) {
  switch (i) {
    case ParamsOrder::x:
    {
      Poincare::Layout x = CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline);
      return ParameterRepresentation{x, I18n::Message::SampleMean};
    }
    case ParamsOrder::s:
    {
      Layout sigma = CodePointLayout::Builder(CodePoint(UCodePointGreekSmallLetterSigma));
      return ParameterRepresentation{sigma, I18n::Message::PopulationStd};
    }
    case ParamsOrder::n:
    {
      Poincare::Layout n = CodePointLayout::Builder('n');
      return ParameterRepresentation{n, I18n::Message::SampleSize};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

ParameterRepresentation OneMean::TParameterRepresentationAtIndex(int i) {
  switch (i) {
    case ParamsOrder::s: {
      Poincare::Layout s = CodePointLayout::Builder('s');
      return ParameterRepresentation{s, I18n::Message::SampleSTD};
    }
    default:
      return OneMean::ZParameterRepresentationAtIndex(i);
  }
}

void OneMean::ComputeZTest(Test * t) {
  double mean = t->m_hypothesisParams.firstParam();
  t->m_testCriticalValue = ComputeTZ(mean, X(t->parametersArray()), S(t->parametersArray()), N(t->parametersArray()));
  t->m_pValue = ComputePValue(t);
}

void OneMean::ComputeTTest(Test * t) {
  t->m_degreesOfFreedom = ComputeDegreesOfFreedom(N(t->parametersArray()));
  ComputeZTest(t);
}

void OneMean::ComputeZInterval(Interval * i) {
  i->m_zCritical = i->computeIntervalCriticalValue();
  i->m_estimate = X(i->parametersArray());
  i->m_SE = ComputeStandardError(S(i->parametersArray()), N(i->parametersArray()));
  i->m_marginOfError = i->m_zCritical * i->m_SE;
}

void OneMean::ComputeTInterval(Interval * i) {
  i->m_degreesOfFreedom = ComputeDegreesOfFreedom(N(i->parametersArray()));
  ComputeZInterval(i);
}


/* OneProportion */

Poincare::Layout OneProportion::EstimateLayout(Poincare::Layout * layout) {
  if (layout->isUninitialized()) {
    *layout = CombinedCodePointsLayout::Builder('p', UCodePointCombiningCircumflex);
  }
  return *layout;
}

void OneProportion::InitTestParameters(Test * t) {
  t->parametersArray()[ParamsOrder::x] = 47;
  t->parametersArray()[ParamsOrder::n] = 500;
  t->m_hypothesisParams.setFirstParam(0.08);
  t->m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);
}

void OneProportion::InitIntervalParameters(Interval * i) {
  i->parametersArray()[ParamsOrder::x] = 107;
  i->parametersArray()[ParamsOrder::n] = 251;
}

bool OneProportion::AuthorizedParameterAtIndex(int i, double p) {
  if (i == NumberOfParameters()) {
    return ValidThreshold(p);
  }
  switch (i) {
    case ParamsOrder::x:
      return p >= 0.0;
    case ParamsOrder::n:
      return p >= 1.0;
    default:
      assert(false);
      return false;
  }
}

double OneProportion::ProcessParamaterForIndex(double p, int index) {
  if (index == ParamsOrder::n || index == ParamsOrder::x) {
    p = std::round(p);
  }
  return p;
}

ParameterRepresentation OneProportion::ParameterRepresentationAtIndex(int i) {
  switch (i) {
    case ParamsOrder::x: {
      Layout x = CodePointLayout::Builder('x');
      return ParameterRepresentation{x, I18n::Message::NumberOfSuccesses};
    }
    case ParamsOrder::n: {
      Layout n = CodePointLayout::Builder('n');
      return ParameterRepresentation{n, I18n::Message::SampleSize};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

void OneProportion::ComputeTest(Test * t) {
  double p0 = t->m_hypothesisParams.firstParam();
  double p = X(t->parametersArray()) / N(t->parametersArray());

  t->m_testCriticalValue = ComputeZ(p0, p, N(t->parametersArray()));
  t->m_pValue = ComputePValue(t);
}

void OneProportion::ComputeInterval(Interval * i) {
  i->m_zCritical = i->computeIntervalCriticalValue();
  i->m_estimate = X(i->parametersArray()) / N(i->parametersArray());
  i->m_SE = ComputeStandardError(i->m_estimate, N(i->parametersArray()));
  i->m_marginOfError = i->m_zCritical * i->m_SE;
}

double OneProportion::ComputeZ(double p0, double p, double n) {
  assert(n > 0 && p0 >= 0 && p >= 0 && p0 <= 1 && p <= 1);
  return (p - p0) / std::sqrt(p0 * (1 - p0) / n);
}

double OneProportion::ComputeStandardError(double pEstimate, double n) {
  return std::sqrt(pEstimate * (1 - pEstimate) / n);
}

/* TwoMeans */

bool TwoMeans::TestInitializeDistribution(Statistic * statistic, DistributionType distributionType) {
  if (!SignificanceTest::InitializeDistribution(statistic, distributionType)) {
    return false;
  }
  statistic->~Statistic();
  switch (distributionType) {
    case DistributionType::T:
      new (statistic) TwoMeansTTest();
      break;
    case DistributionType::TPooled:
      new (statistic) PooledTwoMeansTTest();
      break;
    case DistributionType::Z:
      new (statistic) TwoMeansZTest();
      break;
    default:
      assert(false);
      break;
  }
  statistic->initParameters();
  return true;
}

bool TwoMeans::IntervalInitializeDistribution(Statistic * statistic, DistributionType distributionType) {
  if (!SignificanceTest::InitializeDistribution(statistic, distributionType)) {
    return false;
  }
  statistic->~Statistic();
  switch (distributionType) {
    case DistributionType::T:
      new (statistic) TwoMeansTInterval();
      break;
    case DistributionType::TPooled:
      new (statistic) PooledTwoMeansTInterval();
      break;
    case DistributionType::Z:
      new (statistic) TwoMeansZInterval();
      break;
    default:
      assert(false);
      break;
  }
  statistic->initParameters();
  return true;
}

Poincare::Layout TwoMeans::EstimateLayout(Poincare::Layout * layout) {
  if (layout->isUninitialized()) {
    *layout = XOneMinusXTwoLayout();
  }
  return *layout;
}

void TwoMeans::InitTestParameters(Test * t) {
  t->parametersArray()[ParamsOrder::x1] = 5;
  t->parametersArray()[ParamsOrder::n1] = 10;
  t->parametersArray()[ParamsOrder::s1] = 8.743;
  t->parametersArray()[ParamsOrder::x2] = -0.273;
  t->parametersArray()[ParamsOrder::n2] = 11;
  t->parametersArray()[ParamsOrder::s2] = 5.901;
  t->m_hypothesisParams.setFirstParam(0);
  t->m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);
}

void TwoMeans::InitIntervalParameters(Interval * i) {
  i->parametersArray()[ParamsOrder::x1] = 23.7;
  i->parametersArray()[ParamsOrder::n1] = 30;
  i->parametersArray()[ParamsOrder::s1] = 17.5;
  i->parametersArray()[ParamsOrder::x2] = 34.53;
  i->parametersArray()[ParamsOrder::n2] = 30;
  i->parametersArray()[ParamsOrder::s2] = 14.26;
}

bool TwoMeans::ZAuthorizedParameterAtIndex(int i, double p) {
  switch (i) {
    case ParamsOrder::n1:
    case ParamsOrder::n2:
      return p >= 1.0;
    default:
      return TAuthorizedParameterAtIndex(i, p);
  }
}

bool TwoMeans::TAuthorizedParameterAtIndex(int i, double p) {
  if (i == NumberOfParameters()) {
    return ValidThreshold(p);
  }
  switch (i) {
    case ParamsOrder::n1:
    case ParamsOrder::n2:
      return p >= 2.0;
    case ParamsOrder::s1:
    case ParamsOrder::s2:
      return p >= 0.0;
    default:
      return true;
  }
}

double TwoMeans::ProcessParamaterForIndex(double p, int index) {
  if (index == ParamsOrder::n1 || index == ParamsOrder::n2) {
    p = std::round(p);
    assert(p > 1.0);
  }
  return p;
}

bool TwoMeans::ZValidateInputs(double * params) {
 assert(S1(params) >= 0.0f && S2(params) >= 0.0f);
  return S1(params) >= FLT_MIN || S2(params) >= FLT_MIN;
}

bool TwoMeans::TValidateInputs(double * params) {
  return (ComputeDegreesOfFreedom(S1(params), N1(params), S2(params), N2(params)) > 0) && ZValidateInputs(params);
}

ParameterRepresentation TwoMeans::ZParameterRepresentationAtIndex(int i) {
  switch (i) {
    case ParamsOrder::x1: {
      Poincare::HorizontalLayout x1 = Poincare::HorizontalLayout::Builder(
          CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x1, I18n::Message::Sample1Mean};
    }
    case ParamsOrder::n1: {
      Poincare::HorizontalLayout n1 = LayoutHelper::CodePointSubscriptCodePointLayout('n', '1');
      return ParameterRepresentation{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::s1: {
      Poincare::HorizontalLayout sigma1 = LayoutHelper::CodePointSubscriptCodePointLayout(
          CodePoint(UCodePointGreekSmallLetterSigma),
          '1');
      return ParameterRepresentation{sigma1, I18n::Message::Population1Std};
    }
    case ParamsOrder::x2: {
      Poincare::HorizontalLayout x2 = Poincare::HorizontalLayout::Builder(
          CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x2, I18n::Message::Sample2Mean};
    }
    case ParamsOrder::n2: {
      Poincare::HorizontalLayout n2 = LayoutHelper::CodePointSubscriptCodePointLayout('n', '2');
      ;
      return ParameterRepresentation{n2, I18n::Message::Sample2Size};
    }
    case ParamsOrder::s2: {
      Poincare::HorizontalLayout sigma2 = LayoutHelper::CodePointSubscriptCodePointLayout(
          CodePoint(UCodePointGreekSmallLetterSigma),
          '2');
      return ParameterRepresentation{sigma2, I18n::Message::Population2Std};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

ParameterRepresentation TwoMeans::TParameterRepresentationAtIndex(int i) {
  switch (i) {
    case ParamsOrder::s1: {
      Poincare::HorizontalLayout s1 = LayoutHelper::CodePointSubscriptCodePointLayout('s', '1');
      return ParameterRepresentation{s1, I18n::Message::Sample1Std};
    }
    case ParamsOrder::s2: {
      Poincare::HorizontalLayout s2 = LayoutHelper::CodePointSubscriptCodePointLayout('s', '2');
      return ParameterRepresentation{s2, I18n::Message::Sample2Std};
    }
    default:
      return ZParameterRepresentationAtIndex(i);
  }
}

void TwoMeans::ComputeZTest(Test * t) {
  double deltaMean = t->m_hypothesisParams.firstParam();
  t->m_testCriticalValue = ComputeTZ(deltaMean, X1(t->parametersArray()), N1(t->parametersArray()), S1(t->parametersArray()), X2(t->parametersArray()), N2(t->parametersArray()), S2(t->parametersArray()));
  t->m_pValue = ComputePValue(t);
}

void TwoMeans::ComputeTTest(Test * t) {
  t->m_degreesOfFreedom = ComputeDegreesOfFreedom(S1(t->parametersArray()), N1(t->parametersArray()), S2(t->parametersArray()), N2(t->parametersArray()));
  ComputeZTest(t);
}

void TwoMeans::ComputeZInterval(Interval * i) {
  i->m_zCritical = i->computeIntervalCriticalValue();
  i->m_estimate = X1(i->parametersArray()) - X2(i->parametersArray());
  i->m_SE = ComputeStandardError(S1(i->parametersArray()), N1(i->parametersArray()), S2(i->parametersArray()), N2(i->parametersArray()));
  i->m_marginOfError = i->m_zCritical * i->m_SE;
}

void TwoMeans::ComputeTInterval(Interval * i) {
  i->m_degreesOfFreedom = ComputeDegreesOfFreedom(S1(i->parametersArray()), N1(i->parametersArray()), S2(i->parametersArray()), N2(i->parametersArray()));
  ComputeZInterval(i);
}

double TwoMeans::ComputeTZ(double deltaMean, double meanSample1, double n1, double sigma1, double meanSample2, double n2, double sigma2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) / ComputeStandardError(sigma1, n1, sigma2, n2);
}

double TwoMeans::ComputeStandardError(double s1, int n1, double s2, int n2) {
  return std::sqrt((s1 * s1 / n1 + s2 * s2 / n2));
}

double TwoMeans::ComputeDegreesOfFreedom(double s1, double n1, double s2, double n2) {
  double v1 = std::pow(s1, 2.) / n1;
  double v2 = std::pow(s2, 2.) / n2;
  return std::pow(v1 + v2, 2.) / (std::pow(v1, 2.) / (n1 - 1.) + std::pow(v2, 2.) / (n2 - 1.));
}

/* PooledTwoMeans */

void PooledTwoMeans::ComputeTest(Test * t) {
  double * params = t->parametersArray();
  double n1 = N1(params);
  double s1 = S1(params);
  double x1 = X1(params);
  double n2 = N2(params);
  double s2 = S2(params);
  double x2 = X2(params);
  t->m_degreesOfFreedom = n1 + n2 - 2.;
  double deltaMean = t->m_hypothesisParams.firstParam();
  t->m_testCriticalValue = ((x1 - x2) - (deltaMean)) / ComputeStandardError(n1, s1, n2, s2);
  t->m_pValue = ComputePValue(t);
}

void PooledTwoMeans::ComputeInterval(Interval * i) {
  double * params = i->parametersArray();
  double n1 = N1(params);
  double s1 = S1(params);
  double x1 = X1(params);
  double n2 = N2(params);
  double s2 = S2(params);
  double x2 = X2(params);
  i->m_degreesOfFreedom = n1 + n2 - 2.;
  i->m_zCritical = i->computeIntervalCriticalValue();
  i->m_estimate = x1 - x2;
  i->m_SE = ComputeStandardError(n1, s1, n2, s2);
  i->m_marginOfError = i->m_zCritical * i->m_SE;
}

double PooledTwoMeans::ComputeStandardError(double n1, double s1, double n2, double s2) {
  /* Pooled estimate of sigma */
  double sp = std::sqrt(((n1 - 1.) * s1 * s1 + (n2 - 1.) * s2 * s2) / (n1 + n2 - 2.));
  return sp * std::sqrt(1. / n1 + 1. / n2);
}

/* TwoProportions */

Poincare::Layout TwoProportions::P2Layout(Poincare::Layout * layout) {
  if (layout->isUninitialized()) {
    *layout = std::move(Poincare::HorizontalLayout::Builder(
        CombinedCodePointsLayout::Builder('p', UCodePointCombiningCircumflex),
        VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'),
          VerticalOffsetLayoutNode::Position::Subscript)));
  }
  return *layout;
}

Poincare::Layout TwoProportions::EstimateLayout(Poincare::Layout * layout) {
  if (layout->isUninitialized()) {
    Poincare::HorizontalLayout p1 = Poincare::HorizontalLayout::Builder(
        CombinedCodePointsLayout::Builder('p', UCodePointCombiningCircumflex),
        VerticalOffsetLayout::Builder(CodePointLayout::Builder('1'),
          VerticalOffsetLayoutNode::Position::Subscript));
    Poincare::HorizontalLayout p2 = Poincare::HorizontalLayout::Builder(
        CombinedCodePointsLayout::Builder('p', UCodePointCombiningCircumflex),
        VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'),
          VerticalOffsetLayoutNode::Position::Subscript));
    /* we build a nested layout instead of a flat one to be able to retrieve p1
       and p2 from it when needed by the estimates of the TwoProportionTest */
    Poincare::HorizontalLayout res = Poincare::HorizontalLayout::Builder(
      p1, CodePointLayout::Builder('-'), p2);
    *layout = std::move(res);
  }
  return *layout;
}

void TwoProportions::InitTestParameters(Test * t) {
  t->parametersArray()[ParamsOrder::x1] = 19;
  t->parametersArray()[ParamsOrder::n1] = 80;
  t->parametersArray()[ParamsOrder::x2] = 26;
  t->parametersArray()[ParamsOrder::n2] = 150;
  t->m_hypothesisParams.setFirstParam(0);
  t->m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Different);
}

void TwoProportions::InitIntervalParameters(Interval * i) {
  i->parametersArray()[ParamsOrder::x1] = 639;
  i->parametersArray()[ParamsOrder::n1] = 799;
  i->parametersArray()[ParamsOrder::x2] = 1555;
  i->parametersArray()[ParamsOrder::n2] = 2253;
}

bool TwoProportions::AuthorizedParameterAtIndex(int i, double p) {
  if (i == NumberOfParameters()) {
    return ValidThreshold(p);
  }
  switch (i) {
    case ParamsOrder::x1:
    case ParamsOrder::x2:
    case ParamsOrder::n1:
    case ParamsOrder::n2:
      return p >= 0.0;
    default:
      assert(false);
      return false;
  }
}

double TwoProportions::ProcessParamaterForIndex(double p, int index) {
  if (index == ParamsOrder::n1 || index == ParamsOrder::n2 || index == ParamsOrder::x1 ||
      index == ParamsOrder::x2) {
    p = std::round(p);
    assert(p >= 0.0);
  }
  return p;
}

bool TwoProportions::ValidateInputs(double * params) {
  assert(X1(params) >= 0.0f && X2(params) >= 0.0f);
  if (X1(params) < FLT_MIN && X2(params) < FLT_MIN) {
    return false;
  }
  return (X1(params) < N1(params)) && (X2(params) < N2(params));
}

ParameterRepresentation TwoProportions::ParameterRepresentationAtIndex(int i) {
  switch (i) {
    case ParamsOrder::x1: {
      Poincare::HorizontalLayout x1 = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('x'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x1, I18n::Message::SuccessSample1};
    }
    case ParamsOrder::n1: {
      Poincare::HorizontalLayout n1 = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::x2: {
      Poincare::HorizontalLayout x2 = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('x'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x2, I18n::Message::SuccessSample2};
    }
    case ParamsOrder::n2: {
      Poincare::HorizontalLayout n2 = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{n2, I18n::Message::Sample2Size};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

void TwoProportions::ComputeTest(Test * t) {
  double deltaP0 = t->m_hypothesisParams.firstParam();
  t->m_testCriticalValue = ComputeZ(deltaP0, X1(t->parametersArray()), N1(t->parametersArray()), X2(t->parametersArray()), N2(t->parametersArray()));
  t->m_pValue = ComputePValue(t);
}

void TwoProportions::ComputeInterval(Interval * i) {
  i->m_zCritical = i->computeIntervalCriticalValue();
  i->m_estimate = ComputeEstimate(X1(i->parametersArray()), N1(i->parametersArray()), X2(i->parametersArray()), N2(i->parametersArray()));
  i->m_SE = ComputeStandardError(X1(i->parametersArray()), N1(i->parametersArray()), X2(i->parametersArray()), N2(i->parametersArray()));
  i->m_marginOfError = i->m_zCritical * i->m_SE;
}

double TwoProportions::ComputeZ(double deltaP0, double x1, int n1, double x2, int n2) {
  double p1 = x1 / n1;
  double p2 = x2 / n2;
  double p = (x1 + x2) / (n1 + n2);
  return (p1 - p2 - deltaP0) / std::sqrt(p * (1 - p) * (1. / n1 + 1. / n2));
}
double TwoProportions::ComputeStandardError(double x1, int n1, double x2, int n2) {
  double p1Estimate = x1 / n1;
  double p2Estimate = x2 / n2;
  return std::sqrt(p1Estimate * (1 - p1Estimate) / n1 + p2Estimate * (1 - p2Estimate) / n2);
}

}
