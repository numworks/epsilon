#include "statistic_curve_view.h"

#include <poincare/print_float.h>

#include <algorithm>
#include <cmath>

#include "probability/app.h"

namespace Probability {

constexpr KDColor StatisticCurveView::k_backgroundColor;

void StatisticCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), k_backgroundColor);
  drawAxis(ctx, rect, Axis::Horizontal);

  if (App::app()->subapp() == Data::SubApp::Tests) {
    drawTest(ctx, rect);
  } else {
    drawInterval(ctx, rect);
  }
}

char * StatisticCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

bool StatisticCurveView::shouldDrawLabelAtPosition(float labelValue) const {
  if (App::app()->subapp() == Data::SubApp::Intervals) {
    return true;
  }
  float margin = k_marginsAroundZLabel / static_cast<float>(bounds().width()) *
                 (curveViewRange()->xMax() - curveViewRange()->xMin());
  float z = m_statistic->testCriticalValue();
  return labelValue >= z + margin || labelValue <= z - margin;
}

void StatisticCurveView::drawTest(KDContext * ctx, KDRect rect) const {
  float z = m_statistic->testCriticalValue();
  float zAlpha = m_statistic->zAlpha();

  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);
  drawZLabelAndGraduation(ctx, z);
  colorUnderCurve(ctx, rect, m_statistic->hypothesisParams()->op(), z, zAlpha);
  drawCartesianCurve(ctx,
                     rect,
                     -INFINITY,
                     INFINITY,
                     evaluateTestAtAbsissa,
                     m_statistic,
                     nullptr,
                     Escher::Palette::GrayVeryDark);
}

void StatisticCurveView::drawInterval(KDContext * ctx, KDRect rect) const {
  drawCartesianCurve(ctx,
                     rect,
                     -INFINITY,
                     INFINITY,
                     evaluateIntervalAtAbsissa,
                     m_statistic,
                     nullptr,
                     Escher::Palette::YellowDark,
                     true,
                     true,
                     m_statistic->estimate() - m_statistic->marginOfError(),
                     m_statistic->estimate() + m_statistic->marginOfError());
  drawIntervalLabelAndGraduation(ctx);
}

void StatisticCurveView::colorUnderCurve(KDContext * ctx,
                                         KDRect rect,
                                         HypothesisParams::ComparisonOperator op,
                                         float z,
                                         float zAlpha) const {
  if (op == HypothesisParams::ComparisonOperator::Different) {
    // Recurse for both colors
    z = std::fabs(z);
    assert(zAlpha > 0);
    colorUnderCurve(ctx, rect, HypothesisParams::ComparisonOperator::Higher, z, zAlpha);
    colorUnderCurve(ctx, rect, HypothesisParams::ComparisonOperator::Lower, -z, -zAlpha);
    return;
  }

  float min = std::min(z, zAlpha);
  float max = std::max(z, zAlpha);
  bool zMoreExtreme = op == HypothesisParams::ComparisonOperator::Higher ? z > zAlpha : z < zAlpha;
  KDColor middleColor = !zMoreExtreme ? Escher::Palette::YellowDark : Escher::Palette::GrayMiddle;
  KDColor externColor = zMoreExtreme ? Escher::Palette::YellowDark : Escher::Palette::GrayMiddle;
  drawCartesianCurve(ctx,
                     rect,
                     min,
                     max,
                     evaluateTestAtAbsissa,
                     m_statistic,
                     nullptr,
                     middleColor,
                     true,
                     true,
                     min,
                     max);
  float xmin, xmax;
  if (op == HypothesisParams::ComparisonOperator::Higher) {
    xmin = max;
    xmax = INFINITY;
  } else {
    xmin = -INFINITY;
    xmax = min;
  }
  drawCartesianCurve(ctx,
                     rect,
                     xmin,
                     xmax,
                     evaluateTestAtAbsissa,
                     m_statistic,
                     nullptr,
                     externColor,
                     true,
                     true,
                     xmin,
                     xmax);
}

void StatisticCurveView::drawLabelAndGraduationAtPosition(KDContext * ctx, float position) const {
  assert(std::isfinite(position));
  float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
  KDCoordinate graduationPosition = std::round(floatToPixel(Axis::Horizontal, position));
  // Graduation
  KDRect graduation = KDRect(graduationPosition,
                             verticalOrigin - (k_labelGraduationLength - 2) / 2,
                             1,
                             k_labelGraduationLength);
  ctx->fillRect(graduation, KDColorBlack);
  // Label
  char buffer[k_labelBufferMaxSize];
  Poincare::PrintFloat::ConvertFloatToText<float>(position,
                                                  buffer,
                                                  k_labelBufferMaxSize,
                                                  k_labelBufferMaxGlyphLength,
                                                  k_numberSignificantDigits,
                                                  Poincare::Preferences::PrintFloatMode::Decimal);
  KDPoint labelPosition = positionLabel(graduationPosition,
                                        verticalOrigin,
                                        KDFont::SmallFont->stringSize(buffer),
                                        RelativePosition::None,
                                        RelativePosition::Before);
  ctx->drawString(buffer, labelPosition, KDFont::SmallFont, KDColorBlack, k_backgroundColor);
}

void StatisticCurveView::drawZLabelAndGraduation(KDContext * ctx, float z) const {
  float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
  KDCoordinate labelPosition = std::round(floatToPixel(Axis::Horizontal, z));

  // Graduation
  KDRect graduation = KDRect(labelPosition,
                             verticalOrigin - (k_labelGraduationLength - 2) / 2,
                             1,
                             k_labelGraduationLength);
  ctx->fillRect(graduation, KDColorBlack);

  // Label
  if (z < curveViewRange()->xMin() || z > curveViewRange()->xMax()) {
    // z outside screen
    return;
  }
  const char * zText = "z";

  KDPoint position = positionLabel(labelPosition,
                                   verticalOrigin,
                                   KDFont::SmallFont->stringSize(zText),
                                   RelativePosition::None,
                                   RelativePosition::Before);
  ctx->drawString(zText, position, KDFont::SmallFont, KDColorBlack, k_backgroundColor);
}

void StatisticCurveView::drawIntervalLabelAndGraduation(KDContext * ctx) const {
  drawLabelAndGraduationAtPosition(ctx, m_statistic->estimate());
  if (std::isfinite(m_statistic->marginOfError())) {  // Can be INF if confidence level = 1
    float lowerBound = m_statistic->estimate() - m_statistic->marginOfError();
    float upperBound = m_statistic->estimate() + m_statistic->marginOfError();
    drawLabelAndGraduationAtPosition(ctx, lowerBound);
    drawLabelAndGraduationAtPosition(ctx, upperBound);
  }
}

Poincare::Coordinate2D<float> StatisticCurveView::evaluateTestAtAbsissa(float x,
                                                                        void * model,
                                                                        void * context) {
  Statistic * statistic = static_cast<Statistic *>(model);
  return Poincare::Coordinate2D<float>(x, statistic->normalizedDensityFunction(x));
}

Poincare::Coordinate2D<float> StatisticCurveView::evaluateIntervalAtAbsissa(float x,
                                                                            void * model,
                                                                            void * context) {
  Statistic * statistic = static_cast<Statistic *>(model);
  return Poincare::Coordinate2D<float>(x, statistic->densityFunction(x));
}

}  // namespace Probability
