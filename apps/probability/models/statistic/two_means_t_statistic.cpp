#include "two_means_t_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>
#include <float.h>

#include "probability/text_helpers.h"

using namespace Poincare;

namespace Probability {

void TwoMeansTStatistic::tidy() {
  m_estimateLayout = Layout();
}

void TwoMeansTStatistic::init(Data::SubApp subapp) {
  if (subapp == Data::SubApp::Tests) {
    m_params[ParamsOrder::X1] = 5;
    m_params[ParamsOrder::N1] = 10;
    m_params[ParamsOrder::S1] = 8.743;
    m_params[ParamsOrder::X2] = -0.273;
    m_params[ParamsOrder::N2] = 11;
    m_params[ParamsOrder::S2] = 5.901;
    m_hypothesisParams.setFirstParam(0);
    m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);
  } else {
    m_params[ParamsOrder::X1] = 23.7;
    m_params[ParamsOrder::N1] = 30;
    m_params[ParamsOrder::S1] = 17.5;
    m_params[ParamsOrder::X2] = 34.53;
    m_params[ParamsOrder::N2] = 30;
    m_params[ParamsOrder::S2] = 14.26;
  }
}

bool TwoMeansTStatistic::isValidParamAtIndex(int i, double p) {
  switch (i) {
    case ParamsOrder::N1:
    case ParamsOrder::N2:
      return p >= 2.0;
    case ParamsOrder::S1:
    case ParamsOrder::S2:
      return p >= 0.0;
  }
  return TStatistic::isValidParamAtIndex(i, p);
}

void TwoMeansTStatistic::computeTest() {
  double deltaMean = m_hypothesisParams.firstParam();
  m_degreesOfFreedom = computeDegreesOfFreedom(s1(), n1(), s2(), n2());
  m_testCriticalValue = computeT(deltaMean, x1(), n1(), s1(), x2(), n2(), s2());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.comparisonOperator());
}

void TwoMeansTStatistic::computeInterval() {
  m_degreesOfFreedom = computeDegreesOfFreedom(s1(), n1(), s2(), n2());
  m_estimate = _xEstimate(x1(), x2());
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(s1(), n1(), s2(), n2());
  m_marginOfError = computeMarginOfError(m_SE, m_zCritical);
}

Poincare::Layout TwoMeansTStatistic::estimateLayout() {
  if (m_estimateLayout.isUninitialized()) {
    m_estimateLayout = XOneMinusXTwoLayout();
  }
  return m_estimateLayout;
}

void TwoMeansTStatistic::setParamAtIndex(int index, double p) {
  if (index == ParamsOrder::N1 || index == ParamsOrder::N2) {
    p = std::round(p);
    assert(p > 1.0);
  }
  TStatistic::setParamAtIndex(index, p);
}

ParameterRepresentation TwoMeansTStatistic::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X1: {
      Poincare::HorizontalLayout x1 = Poincare::HorizontalLayout::Builder(
          CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x1, I18n::Message::Sample1Mean};
    }
    case ParamsOrder::S1: {
      Poincare::HorizontalLayout s1 = codePointSubscriptCodePointLayout('s', '1');
      return ParameterRepresentation{s1, I18n::Message::Sample1Std};
    }
    case ParamsOrder::N1: {
      Poincare::HorizontalLayout n1 = codePointSubscriptCodePointLayout('n', '1');
      return ParameterRepresentation{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::X2: {
      Poincare::HorizontalLayout x2 = Poincare::HorizontalLayout::Builder(
          CombinedCodePointsLayout::Builder('x', UCodePointCombiningOverline),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x2, I18n::Message::Sample2Mean};
    }
    case ParamsOrder::S2: {
      Poincare::HorizontalLayout s2 = codePointSubscriptCodePointLayout('s', '2');
      return ParameterRepresentation{s2, I18n::Message::Sample2Std};
    }
    case ParamsOrder::N2: {
      Poincare::HorizontalLayout n2 = codePointSubscriptCodePointLayout('n', '2');
      return ParameterRepresentation{n2, I18n::Message::Sample2Size};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

bool TwoMeansTStatistic::validateInputs() {
  // TODO: factorize with TwoMeansZStatistic
  assert(s1() >= 0.0f && s2() >= 0.0f);
  return s1() >= FLT_MIN || s2() >= FLT_MIN;
}

double TwoMeansTStatistic::_xEstimate(double meanSample1, double meanSample2) {
  return meanSample1 - meanSample2;
}

double TwoMeansTStatistic::computeT(double deltaMean,
                                   double meanSample1,
                                   double n1,
                                   double s1,
                                   double meanSample2,
                                   double n2,
                                   double s2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) / computeStandardError(s1, n1, s2, n2);
}

double TwoMeansTStatistic::computeDegreesOfFreedom(double s1, double n1, double s2, double n2) {
  double v1 = pow(s1, 2) / n1;
  double v2 = pow(s2, 2) / n2;
  return pow(v1 + v2, 2) / (pow(v1, 2) / (n1 - 1) + pow(v2, 2) / (n2 - 1));
}

double TwoMeansTStatistic::computeStandardError(double s1, double n1, double s2, double n2) {
  return std::sqrt((s1 * s1 / n1 + s2 * s2 / n2));
}

}  // namespace Probability
