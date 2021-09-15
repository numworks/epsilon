#include "statistic_curve_view.h"

#include <poincare/absolute_value_layout.h>
#include <poincare/opposite.h>
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
  bool res = labelValue >= z + margin || labelValue <= z - margin;
  if (m_statistic->hypothesisParams()->comparisonOperator() ==
      HypothesisParams::ComparisonOperator::Different) {
    res = res && (labelValue >= -z + margin || labelValue <= -z - margin);
  }
  return res;
}

void StatisticCurveView::drawTest(KDContext * ctx, KDRect rect) const {
  float z = m_statistic->testCriticalValue();

  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);
  drawZLabelAndZGraduation(ctx, z);
  colorUnderCurve(ctx, rect, m_statistic->hypothesisParams()->comparisonOperator(), z);
}

void StatisticCurveView::drawInterval(KDContext * ctx, KDRect rect) const {
  drawCartesianCurve(ctx,
                     rect,
                     -INFINITY,
                     INFINITY,
                     evaluateIntervalAtAbscissa,
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
                                         float z) const {
  if (op == HypothesisParams::ComparisonOperator::Different) {
    // Recurse for both colors
    z = std::fabs(z);
    // TODO optimize ?
    colorUnderCurve(ctx, rect, HypothesisParams::ComparisonOperator::Higher, z);
    colorUnderCurve(ctx, rect, HypothesisParams::ComparisonOperator::Lower, -z);
    return;
  }

  float min = op == HypothesisParams::ComparisonOperator::Higher ? z : -INFINITY;
  float max = op == HypothesisParams::ComparisonOperator::Higher ? INFINITY : z;
  drawCartesianCurve(ctx,
                     rect,
                     -INFINITY,
                     INFINITY,
                     evaluateTestAtAbscissa,
                     m_statistic,
                     nullptr,
                     Escher::Palette::YellowDark,
                     true,
                     true,
                     min,
                     max);
}

void StatisticCurveView::drawLabelAndGraduationAtPosition(KDContext * ctx,
                                                          float position,
                                                          const char * text) const {
  // Draw only if visible
  if ((curveViewRange()->xMin() <= position) && (position <= curveViewRange()->xMax())) {
    float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
    KDCoordinate graduationPosition = drawGraduationAtPosition(ctx, position);

    // Label
    KDPoint labelPosition = positionLabel(graduationPosition,
                                          verticalOrigin,
                                          KDFont::SmallFont->stringSize(text),
                                          RelativePosition::None,
                                          RelativePosition::Before);
    ctx->drawString(text, labelPosition, KDFont::SmallFont, KDColorBlack, k_backgroundColor);
  }
}

void StatisticCurveView::drawLabelAndGraduationAtPosition(KDContext * ctx,
                                                          float position,
                                                          Poincare::Layout symbol) const {
  if ((curveViewRange()->xMin() <= position) && (position <= curveViewRange()->xMax())) {
    Poincare::Layout small = setSmallFont(symbol.clone());
    float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
    KDCoordinate graduationPosition = drawGraduationAtPosition(ctx, position);

    KDPoint labelPosition = positionLabel(graduationPosition,
                                          verticalOrigin,
                                          small.layoutSize(),
                                          RelativePosition::None,
                                          RelativePosition::Before);
    small.draw(ctx, labelPosition, KDColorBlack, k_backgroundColor);
  }
}

void StatisticCurveView::drawZLabelAndZGraduation(KDContext * ctx, float z) const {
  // Label
  if (z < curveViewRange()->xMin() || z > curveViewRange()->xMax()) {
    // z outside screen
    return;
  }
  if (m_statistic->hypothesisParams()->comparisonOperator() ==
      HypothesisParams::ComparisonOperator::Different) {
    Poincare::AbsoluteValueLayout absolute = Poincare::AbsoluteValueLayout::Builder(
        m_statistic->testCriticalValueSymbol());
    drawLabelAndGraduationAtPosition(ctx, std::abs(z), absolute);
    drawLabelAndGraduationAtPosition(
        ctx,
        -std::abs(z),
        Poincare::HorizontalLayout::Builder(Poincare::CodePointLayout::Builder('-'), absolute));
  } else {
    drawLabelAndGraduationAtPosition(ctx, z, m_statistic->testCriticalValueSymbol());
  }
}

void StatisticCurveView::drawIntervalLabelAndGraduation(KDContext * ctx) const {
  char buffer[k_labelBufferMaxSize];
  convertFloatToText(m_statistic->estimate(), buffer, k_labelBufferMaxSize);
  drawLabelAndGraduationAtPosition(ctx, m_statistic->estimate(), buffer);
  float lowerBound = m_statistic->estimate() - m_statistic->marginOfError();
  float upperBound = m_statistic->estimate() + m_statistic->marginOfError();
  convertFloatToText(lowerBound, buffer, k_labelBufferMaxSize);
  drawLabelAndGraduationAtPosition(ctx, lowerBound, buffer);
  convertFloatToText(upperBound, buffer, k_labelBufferMaxSize);
  drawLabelAndGraduationAtPosition(ctx, upperBound, buffer);
}

KDCoordinate StatisticCurveView::drawGraduationAtPosition(KDContext * ctx, float position) const {
  float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
  KDCoordinate graduationPosition = std::round(floatToPixel(Axis::Horizontal, position));
  // Graduation
  KDRect graduation = KDRect(graduationPosition,
                             verticalOrigin - (k_labelGraduationLength - 2) / 2,
                             1,
                             k_labelGraduationLength);
  ctx->fillRect(graduation, KDColorBlack);
  return graduationPosition;
}

void StatisticCurveView::convertFloatToText(float value, char * buffer, int bufferSize) const {
  Poincare::PrintFloat::ConvertFloatToText<float>(value,
                                                  buffer,
                                                  k_labelBufferMaxSize,
                                                  k_labelBufferMaxGlyphLength,
                                                  k_numberSignificantDigits,
                                                  Poincare::Preferences::PrintFloatMode::Decimal);
}

Poincare::Coordinate2D<float> StatisticCurveView::evaluateTestAtAbscissa(float x,
                                                                         void * model,
                                                                         void * context) {
  Statistic * statistic = static_cast<Statistic *>(model);
  return Poincare::Coordinate2D<float>(x, statistic->canonicalDensityFunction(x));
}

Poincare::Coordinate2D<float> StatisticCurveView::evaluateIntervalAtAbscissa(float x,
                                                                             void * model,
                                                                             void * context) {
  Statistic * statistic = static_cast<Statistic *>(model);
  return Poincare::Coordinate2D<float>(x, statistic->densityFunction(x));
}

}  // namespace Probability
