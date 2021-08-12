#include "two_proportions_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

TwoProportionsStatistic::TwoProportionsStatistic() {
  m_params[ParamsOrder::X1] = 20;
  m_params[ParamsOrder::N1] = 50;
  m_params[ParamsOrder::X2] = 40;
  m_params[ParamsOrder::N2] = 60;
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
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.op());
  m_testCriticalValue = computeZ(deltaP0, x1(), n1(), x2(), n2());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.op());
}

void TwoProportionsStatistic::computeInterval() {
  m_estimate = computeEstimate(x1(), n1(), x2(), n2());
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(x1(), n1(), x2(), n2());
  m_ME = computeMarginOfError(m_zCritical, m_SE);
}

Poincare::Layout TwoProportionsStatistic::estimateLayout() {
  // TODO add ^
  HorizontalLayout p1 = HorizontalLayout::Builder(
      CodePointLayout::Builder('p'),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('1'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout p2 = HorizontalLayout::Builder(
      CodePointLayout::Builder('p'),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout res = HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  res.addOrMergeChildAtIndex(p2, 1, true);
  res.addOrMergeChildAtIndex(p1, 0, true);
  return std::move(res);
}

ParameterRepr TwoProportionsStatistic::paramReprAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X1: {
      HorizontalLayout x1 = HorizontalLayout::Builder(
          CodePointLayout::Builder('x'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{x1, I18n::Message::SuccessSample1};
    }
    case ParamsOrder::N1: {
      HorizontalLayout n1 = HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::X2: {
      HorizontalLayout x2 = HorizontalLayout::Builder(
          CodePointLayout::Builder('x'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{x2, I18n::Message::SuccessSample2};
    }
    case ParamsOrder::N2: {
      HorizontalLayout n2 = HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{n2, I18n::Message::Sample2Size};
    }
  }
  assert(false);
  return ParameterRepr{};
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
