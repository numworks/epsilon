#include "two_means_z_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>
#include <float.h>

#include "probability/text_helpers.h"

using namespace Poincare;

namespace Probability {

void TwoMeansZStatistic::tidy() {
  m_estimateLayout = Layout();
}

void TwoMeansZStatistic::init(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Tests) {
    m_params[ParamsOrder::X1] = 5;
    m_params[ParamsOrder::N1] = 10;
    m_params[ParamsOrder::Sigma1] = 8.743;
    m_params[ParamsOrder::X2] = -0.273;
    m_params[ParamsOrder::N2] = 11;
    m_params[ParamsOrder::Sigma2] = 5.901;
    m_hypothesisParams.setFirstParam(0);
    m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);
  } else {
    m_params[ParamsOrder::X1] = 23.7;
    m_params[ParamsOrder::N1] = 30;
    m_params[ParamsOrder::Sigma1] = 17.5;
    m_params[ParamsOrder::X2] = 34.53;
    m_params[ParamsOrder::N2] = 30;
    m_params[ParamsOrder::Sigma2] = 14.26;
  }
}

bool TwoMeansZStatistic::isValidParamAtIndex(int i, double p) {
  switch (i) {
    case ParamsOrder::N1:
    case ParamsOrder::N2:
      return p >= 1.0;
    case ParamsOrder::Sigma1:
    case ParamsOrder::Sigma2:
      return p >= 0;
  }
  return ZStatistic::isValidParamAtIndex(i, p);
}

void TwoMeansZStatistic::computeTest() {
  double deltaMean = m_hypothesisParams.firstParam();
  m_testCriticalValue = computeZ(deltaMean, x1(), n1(), sigma1(), x2(), n2(), sigma2());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.comparisonOperator());
}

void TwoMeansZStatistic::computeInterval() {
  m_estimate = _xEstimate(x1(), x2());
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(sigma1(), n1(), sigma2(), n2());
  m_marginOfError = computeMarginOfError(m_zCritical, m_SE);
}

Poincare::Layout TwoMeansZStatistic::estimateLayout() {
  if (m_estimateLayout.isUninitialized()) {
    m_estimateLayout = XOneMinusXTwoLayout();
  }
  return m_estimateLayout;
}

void TwoMeansZStatistic::setParamAtIndex(int index, double p) {
  if (index == ParamsOrder::N1 || index == ParamsOrder::N2) {
    p = std::round(p);
    assert(p > 0.0);
  }
  ZStatistic::setParamAtIndex(index, p);
}

ParameterRepresentation TwoMeansZStatistic::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X1: {
      Poincare::HorizontalLayout x1 = Poincare::HorizontalLayout::Builder(
          CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x1, I18n::Message::Sample1Mean};
    }
    case ParamsOrder::N1: {
      Poincare::HorizontalLayout n1 = codePointSubscriptCodePointLayout('n', '1');
      return ParameterRepresentation{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::Sigma1: {
      Poincare::HorizontalLayout sigma1 = codePointSubscriptCodePointLayout(
          CodePoint(UCodePointGreekSmallLetterSigma),
          '1');
      return ParameterRepresentation{sigma1, I18n::Message::Population1Std};
    }
    case ParamsOrder::X2: {
      Poincare::HorizontalLayout x2 = Poincare::HorizontalLayout::Builder(
          CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x2, I18n::Message::Sample2Mean};
    }
    case ParamsOrder::N2: {
      Poincare::HorizontalLayout n2 = codePointSubscriptCodePointLayout('n', '2');
      ;
      return ParameterRepresentation{n2, I18n::Message::Sample1Size};
    }
    case ParamsOrder::Sigma2: {
      Poincare::HorizontalLayout sigma2 = codePointSubscriptCodePointLayout(
          CodePoint(UCodePointGreekSmallLetterSigma),
          '2');
      return ParameterRepresentation{sigma2, I18n::Message::Population2Std};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

bool TwoMeansZStatistic::validateInputs() {
  assert(sigma1() >= 0.0f && sigma2() >= 0.0f);
  return sigma1() >= FLT_MIN || sigma2() >= FLT_MIN;
}

double TwoMeansZStatistic::_xEstimate(double meanSample1, double meanSample2) {
  return meanSample1 - meanSample2;
}

double TwoMeansZStatistic::computeZ(double deltaMean,
                                   double meanSample1,
                                   double n1,
                                   double sigma1,
                                   double meanSample2,
                                   double n2,
                                   double sigma2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) / computeStandardError(sigma1, n1, sigma2, n2);
}

double TwoMeansZStatistic::computeStandardError(double sigma1, int n1, double sigma2, int n2) {
  return std::sqrt((sigma1 * sigma1 / n1 + sigma2 * sigma2 / n2));
}

}  // namespace Probability
