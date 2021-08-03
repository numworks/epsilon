#include "two_means_t_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/conjugate_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

TwoMeansTStatistic::TwoMeansTStatistic() {
  m_params[ParamsOrder::X1] = 20;
  m_params[ParamsOrder::N1] = 50;
  m_params[ParamsOrder::S1] = 2;
  m_params[ParamsOrder::X2] = 19;
  m_params[ParamsOrder::N2] = 50;
  m_params[ParamsOrder::S2] = 2;
}

bool TwoMeansTStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::N1:
    case ParamsOrder::N2:
      return p > 0;
    case ParamsOrder::S1:
    case ParamsOrder::S2:
      return p >= 0;
  }
  return TStatistic::isValidParamAtIndex(i, p);
}

void TwoMeansTStatistic::computeTest() {
  float deltaMean = m_hypothesisParams.firstParam();
  m_degreesOfFreedom = _degreeOfFreedom(s1(), n1(), s2(), n2());
  m_zAlpha = absIfNeeded(_tAlpha(m_degreesOfFreedom, m_threshold));
  m_z = _t(deltaMean, x1(), n1(), s1(), x2(), n2(), s2());
  m_pValue = _pVal(m_degreesOfFreedom, m_z);
}

void TwoMeansTStatistic::computeInterval() {
  m_degreesOfFreedom = _degreeOfFreedom(s1(), n1(), s2(), n2());
  m_pEstimate = _xEstimate(x1(), x2());
  m_zCritical = _tCritical(m_degreesOfFreedom, threshold());
  m_SE = _SE(s1(), n1(), s2(), n2());
  m_ME = _ME(m_SE, m_zCritical);
}

ParameterRepr TwoMeansTStatistic::paramReprAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X1: {
      HorizontalLayout x1 = HorizontalLayout::Builder(
          ConjugateLayout::Builder(CodePointLayout::Builder('x')),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::SmallFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{x1, I18n::Message::Sample1Mean};
    }
    case ParamsOrder::S1: {
      HorizontalLayout s1 = HorizontalLayout::Builder(
          CodePointLayout::Builder('s'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::SmallFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{s1, I18n::Message::Sample1Std};
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
          ConjugateLayout::Builder(CodePointLayout::Builder('x')),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::SmallFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{x2, I18n::Message::Sample2Mean};
    }
    case ParamsOrder::S2: {
      HorizontalLayout s1 = HorizontalLayout::Builder(
          CodePointLayout::Builder('s'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::SmallFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{s1, I18n::Message::Sample2Std};
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

float TwoMeansTStatistic::_xEstimate(float meanSample1, float meanSample2) {
  return meanSample1 - meanSample2;
}

float TwoMeansTStatistic::_t(float deltaMean,
                             float meanSample1,
                             float n1,
                             float s1,
                             float meanSample2,
                             float n2,
                             float s2) {
  return absIfNeeded(((meanSample1 - meanSample2) - (deltaMean)) / _SE(s1, n1, s2, n2));
}

float TwoMeansTStatistic::_degreeOfFreedom(float s1, int n1, float s2, int n2) {
  float v1 = pow(s1, 2) / n1;
  float v2 = pow(s2, 2) / n2;
  return pow(v1 + v2, 2) / (pow(v1, 2) / (n1 - 1) + pow(v2, 2) / (n2 - 1));
}

float TwoMeansTStatistic::_SE(float s1, int n1, float s2, int n2) {
  return sqrt((s1 * s1 / n1 + s2 * s2 / n2));
}

}  // namespace Probability
