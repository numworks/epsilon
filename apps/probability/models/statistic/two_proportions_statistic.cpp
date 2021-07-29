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
  m_zAlpha = absIfNeeded(_zAlpha(m_threshold));
  m_z = _z(deltaP0, x1(), n1(), x2(), n2());
  char op = static_cast<char>(m_hypothesisParams.op());
  m_pValue = _pVal(m_z, op);
}

void TwoProportionsStatistic::computeInterval() {
  m_pEstimate = _pEstimate(x1(), n1(), x2(), n2());
  m_zCritical = _zCritical(threshold());
  m_SE = _SE(x1(), n1(), x2(), n2());
  m_ME = _ME(m_zCritical, m_SE);
}

ParameterRepr TwoProportionsStatistic::paramReprAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X1: {
      HorizontalLayout x1 = HorizontalLayout::Builder(
          CodePointLayout::Builder('x'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::SmallFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{x1, I18n::Message::SuccessSample1};
    }
    case ParamsOrder::N1: {
      HorizontalLayout n1 = HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::SmallFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::X2: {
      HorizontalLayout x2 = HorizontalLayout::Builder(
          CodePointLayout::Builder('x'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::SmallFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{x2, I18n::Message::SuccessSample2};
    }
    case ParamsOrder::N2: {
      HorizontalLayout n2 = HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::SmallFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{n2, I18n::Message::Sample2Size};
    }
  }
  assert(false);
  return ParameterRepr{};
}

float TwoProportionsStatistic::_pEstimate(float x1, float n1, float x2, float n2) {
  return x1 / n1 - x2 / n2;
}

float TwoProportionsStatistic::_z(float deltaP0, float x1, int n1, float x2, int n2) {
  float p1 = x1 / n1;
  float p2 = x2 / n2;
  float p = (x1 + x2) / (n1 + n2);
  return absIfNeeded((p1 - p2 - deltaP0) / sqrt(p * (1 - p) * (1. / n1 + 1. / n2)));
}

float TwoProportionsStatistic::_SE(float x1, int n1, float x2, int n2) {
  float p1Estimate = x1 / n1;
  float p2Estimate = x2 / n2;
  return sqrt(p1Estimate * (1 - p1Estimate) / n1 + p2Estimate * (1 - p2Estimate) / n2);
}

}  // namespace Probability
