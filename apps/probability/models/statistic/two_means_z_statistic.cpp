#include "two_means_z_statistic.h"

#include <poincare/code_point_layout.h>
#include <poincare/conjugate_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/vertical_offset_layout.h>

#include <cmath>

using namespace Poincare;

namespace Probability {

TwoMeansZStatistic::TwoMeansZStatistic() {
  m_params[ParamsOrder::X1] = 20;
  m_params[ParamsOrder::Sigma1] = 2;
  m_params[ParamsOrder::N1] = 10;
  m_params[ParamsOrder::X2] = 21;
  m_params[ParamsOrder::Sigma2] = 2;
  m_params[ParamsOrder::N2] = 12;
}

bool TwoMeansZStatistic::isValidParamAtIndex(int i, float p) {
  switch (i) {
    case ParamsOrder::N1:
    case ParamsOrder::N2:
      return p > 0;
    case ParamsOrder::Sigma1:
    case ParamsOrder::Sigma2:
      return p >= 0;
  }
  return ZStatistic::isValidParamAtIndex(i, p);
}

void TwoMeansZStatistic::computeTest() {
  float deltaMean = m_hypothesisParams.firstParam();
  m_zAlpha = computeZAlpha(m_threshold, m_hypothesisParams.op());
  m_testCriticalValue = computeZ(deltaMean, x1(), n1(), sigma1(), x2(), n2(), sigma2());
  m_pValue = computePValue(m_testCriticalValue, m_hypothesisParams.op());
}

void TwoMeansZStatistic::computeInterval() {
  m_estimate = _xEstimate(x1(), x2());
  m_zCritical = computeIntervalCriticalValue(m_threshold);
  m_SE = computeStandardError(sigma1(), n1(), sigma2(), n2());
  m_ME = computeMarginOfError(m_zCritical, m_SE);
}

Poincare::Layout TwoMeansZStatistic::estimateLayout() {
  // TODO factor with TwoMeansTStat
  HorizontalLayout x1 = HorizontalLayout::Builder(
      ConjugateLayout::Builder(CodePointLayout::Builder('x')),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('1'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout x2 = HorizontalLayout::Builder(
      ConjugateLayout::Builder(CodePointLayout::Builder('x')),
      VerticalOffsetLayout::Builder(CodePointLayout::Builder('2'),
                                    VerticalOffsetLayoutNode::Position::Subscript));
  HorizontalLayout res = HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  res.addOrMergeChildAtIndex(x2, 1, true);
  res.addOrMergeChildAtIndex(x1, 0, true);
  return std::move(res);
}

ParameterRepr TwoMeansZStatistic::paramReprAtIndex(int i) const {
  switch (i) {
    case ParamsOrder::X1: {
      HorizontalLayout x1 = HorizontalLayout::Builder(
          ConjugateLayout::Builder(CodePointLayout::Builder('x')),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{x1, I18n::Message::Sample1Mean};
    }
    case ParamsOrder::N1: {
      HorizontalLayout n1 = HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{n1, I18n::Message::Sample1Size};
    }
    case ParamsOrder::Sigma1: {
      HorizontalLayout sigma1 = HorizontalLayout::Builder(
          CodePointLayout::Builder(CodePoint(UCodePointGreekSmallLetterSigma)),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('1', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{sigma1, I18n::Message::Sample1Std};
    }
    case ParamsOrder::X2: {
      HorizontalLayout x2 = HorizontalLayout::Builder(
          ConjugateLayout::Builder(CodePointLayout::Builder('x')),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{x2, I18n::Message::Sample2Mean};
    }
    case ParamsOrder::N2: {
      HorizontalLayout n2 = HorizontalLayout::Builder(
          CodePointLayout::Builder('n'),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{n2, I18n::Message::Sample1Size};
    }
    case ParamsOrder::Sigma2: {
      HorizontalLayout sigma2 = HorizontalLayout::Builder(
          CodePointLayout::Builder(CodePoint(UCodePointGreekSmallLetterSigma)),
          VerticalOffsetLayout::Builder(CodePointLayout::Builder('2', KDFont::LargeFont),
                                        VerticalOffsetLayoutNode::Position::Subscript));
      return ParameterRepr{sigma2, I18n::Message::Sample2Std};
    }
  }
  assert(false);
  return ParameterRepr{};
}

float TwoMeansZStatistic::_xEstimate(float meanSample1, float meanSample2) {
  return meanSample1 - meanSample2;
}

float TwoMeansZStatistic::computeZ(float deltaMean, float meanSample1, float n1, float sigma1,
                             float meanSample2, float n2, float sigma2) {
  return ((meanSample1 - meanSample2) - (deltaMean)) / computeStandardError(sigma1, n1, sigma2, n2);
}

float TwoMeansZStatistic::computeStandardError(float sigma1, int n1, float sigma2, int n2) {
  return sqrt((sigma1 * sigma1 / n1 + sigma2 * sigma2 / n2));
}

}  // namespace Probability
