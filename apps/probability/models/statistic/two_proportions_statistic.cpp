#include "two_proportions_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

#include "probability/app.h"

using namespace Poincare;

namespace Probability {

TwoProportionsStatistic::TwoProportionsStatistic() {
  if (App::app()->subapp() == Data::SubApp::Tests) {
    m_params[ParamsOrder::X1] = 19;
    m_params[ParamsOrder::N1] = 80;
    m_params[ParamsOrder::X2] = 26;
    m_params[ParamsOrder::N2] = 150;
    m_hypothesisParams.setFirstParam(0);
    m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Different);
  } else {
    m_params[ParamsOrder::X1] = 639;
    m_params[ParamsOrder::N1] = 799;
    m_params[ParamsOrder::X2] = 1555;
    m_params[ParamsOrder::N2] = 2253;
  }
}

bool TwoProportionsStatistic::isValidH0(float p) {
  return p <= 1 && p >= -1;
}

bool TwoProportionsStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::X1:
    case ParamsOrder::X2:
    case ParamsOrder::N1:
    case ParamsOrder::N2:
      return p >= 0;
      break;
  }
  return ZStatistic::isValidParamAtIndex(i, p);
}

void TwoProportionsStatistic::computeTest() {
  float deltaP0 = m_hypothesisParams.firstParam();
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.comparisonOperator());
  m_testCriticalValue = computeZ(deltaP0, x1(), n1(), x2(), n2());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.comparisonOperator());
}

void TwoProportionsStatistic::computeInterval() {
  m_estimate = computeEstimate(x1(), n1(), x2(), n2());
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(x1(), n1(), x2(), n2());
  m_marginOfError = computeMarginOfError(m_zCritical, m_SE);
}

Poincare::Layout TwoProportionsStatistic::estimateLayout() {
  // TODO add ^
  Poincare::HorizontalLayout p1 = Poincare::HorizontalLayout::Builder(
      CodePointLayout::Builder('p'),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('1'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  Poincare::HorizontalLayout p2 = Poincare::HorizontalLayout::Builder(
      CodePointLayout::Builder('p'),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  Poincare::HorizontalLayout res = Poincare::HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  res.addOrMergeChildAtIndex(p2, 1, true);
  res.addOrMergeChildAtIndex(p1, 0, true);
  return std::move(res);
}

void TwoProportionsStatistic::setParamAtIndex(int index, float p) {
  if (index == ParamsOrder::N1 || index == ParamsOrder::N2 || index == ParamsOrder::X1 ||
      index == ParamsOrder::X2) {
    p = std::round(p);
  }
  ZStatistic::setParamAtIndex(index, p);
}

ParameterRepresentation TwoProportionsStatistic::paramRepresentationAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X1: {
      Poincare::HorizontalLayout x1 = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('x'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x1, I18n::Message::SuccessSample1};
    }
    case ParamsOrder::N1: {
      Poincare::HorizontalLayout n1 = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::X2: {
      Poincare::HorizontalLayout x2 = Poincare::HorizontalLayout::Builder(
          CodePointLayout::Builder('x'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepresentation{x2, I18n::Message::SuccessSample2};
    }
    case ParamsOrder::N2: {
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

bool TwoProportionsStatistic::validateInputs() {
  return (x1() < n1()) && (x2() < n2());
}

float TwoProportionsStatistic::computeEstimate(float x1, float n1, float x2, float n2) {
  return x1 / n1 - x2 / n2;
}

float TwoProportionsStatistic::computeZ(float deltaP0, float x1, int n1, float x2, int n2) {
  float p1 = x1 / n1;
  float p2 = x2 / n2;
  float p = (x1 + x2) / (n1 + n2);
  return (p1 - p2 - deltaP0) / sqrt(p * (1 - p) * (1. / n1 + 1. / n2));
}

float TwoProportionsStatistic::computeStandardError(float x1, int n1, float x2, int n2) {
  float p1Estimate = x1 / n1;
  float p2Estimate = x2 / n2;
  return sqrt(p1Estimate * (1 - p1Estimate) / n1 + p2Estimate * (1 - p2Estimate) / n2);
}

}  // namespace Probability
