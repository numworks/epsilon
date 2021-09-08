#include "two_means_t_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/conjugate_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

#include "probability/text_helpers.h"

using namespace Poincare;

namespace Probability {

TwoMeansTStatistic::TwoMeansTStatistic() {
  m_params[ParamsOrder::X1] = 20;
  m_params[ParamsOrder::N1] = 10;
  m_params[ParamsOrder::S1] = 2;
  m_params[ParamsOrder::X2] = 21;
  m_params[ParamsOrder::N2] = 12;
  m_params[ParamsOrder::S2] = 2;
}

bool TwoMeansTStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::N1:
    case ParamsOrder::N2:
      return p > 1;
    case ParamsOrder::S1:
    case ParamsOrder::S2:
      return p >= 0;
  }
  return TStatistic::isValidParamAtIndex(i, p);
}

void TwoMeansTStatistic::computeTest() {
  float deltaMean = m_hypothesisParams.firstParam();
  m_degreesOfFreedom = computeDegreesOfFreedom(s1(), n1(), s2(), n2());
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.comparisonOperator());
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
  return XOneMinusXTwoLayout();
}

void TwoMeansTStatistic::setParamAtIndex(int index, float p) {
  if (index == ParamsOrder::N1 || index == ParamsOrder::N2) {
    p = std::round(p);
  }
  TStatistic::setParamAtIndex(index, p);
}

ParameterRepresentation TwoMeansTStatistic::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X1: {
      HorizontalLayout x1 = HorizontalLayout::Builder(
          ConjugateLayout::Builder(CodePointLayout::Builder('x')),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x1, I18n::Message::Sample1Mean};
    }
    case ParamsOrder::S1: {
      HorizontalLayout s1 = codePointSubscriptCodePointLayout('s', '1');
      return ParameterRepresentation{s1, I18n::Message::Sample1Std};
    }
    case ParamsOrder::N1: {
      HorizontalLayout n1 = codePointSubscriptCodePointLayout('n', '1');
      return ParameterRepresentation{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::X2: {
      HorizontalLayout x2 = HorizontalLayout::Builder(
          ConjugateLayout::Builder(CodePointLayout::Builder('x')),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x2, I18n::Message::Sample2Mean};
    }
    case ParamsOrder::S2: {
      HorizontalLayout s2 = codePointSubscriptCodePointLayout('s', '2');
      return ParameterRepresentation{s2, I18n::Message::Sample2Std};
    }
    case ParamsOrder::N2: {
      HorizontalLayout n2 = codePointSubscriptCodePointLayout('n', '2');
      return ParameterRepresentation{n2, I18n::Message::Sample2Size};
    }
  }
  assert(false);
  return ParameterRepresentation{};
}

float TwoMeansTStatistic::_xEstimate(float meanSample1, float meanSample2) {
  return meanSample1 - meanSample2;
}

float TwoMeansTStatistic::computeT(float deltaMean,
                                   float meanSample1,
                                   float n1,
                                   float s1,
                                   float meanSample2,
                                   float n2,
                                   float s2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) / computeStandardError(s1, n1, s2, n2);
}

float TwoMeansTStatistic::computeDegreesOfFreedom(float s1, float n1, float s2, float n2) {
  float v1 = pow(s1, 2) / n1;
  float v2 = pow(s2, 2) / n2;
  return pow(v1 + v2, 2) / (pow(v1, 2) / (n1 - 1) + pow(v2, 2) / (n2 - 1));
}

float TwoMeansTStatistic::computeStandardError(float s1, float n1, float s2, float n2) {
  return sqrt((s1 * s1 / n1 + s2 * s2 / n2));
}

}  // namespace Probability
