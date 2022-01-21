#include "statistic_curve_view.h"

#include <poincare/absolute_value_layout.h>
#include <poincare/opposite.h>
#include <poincare/print_float.h>
#include <poincare/print.h>

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
  float z = static_cast<float>(m_statistic->testCriticalValue());
  bool res = labelValue >= z + margin || labelValue <= z - margin;
  if (m_statistic->hypothesisParams()->comparisonOperator() ==
      HypothesisParams::ComparisonOperator::Different) {
    res = res && (labelValue >= -z + margin || labelValue <= -z - margin);
  }
  return res;
}

void StatisticCurveView::drawTest(KDContext * ctx, KDRect rect) const {
  float z = static_cast<float>(m_statistic->testCriticalValue());

  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, false, false, false, 0, k_backgroundColor);
  drawZLabelAndZGraduation(ctx, z);
  colorUnderCurve(ctx, rect, m_statistic->hypothesisParams()->comparisonOperator(), z);
}

// Draw the main interval along side with 3 other significant intervals.
void StatisticCurveView::drawInterval(KDContext * ctx, KDRect rect) const {
  /* Distribute the Interval::k_numberOfDisplayedIntervals intervals between top of rect and axis:
   *  i   isMainInterval
   *                                            |10%|
   *  0       false                             |▔▔▔|
   *                                        |    20%    |
   *  1       false                         |▔▔▔▔▔▔▔▔▔▔▔|
   *                                     |      25.4%      |
   *  2       true                       |▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔|
   *                               |            40%                |
   *  3       false                |▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔|
   *
   *  axis                   ------------|-----------------|-------------------
   *  */
  float top = pixelToFloat(Axis::Vertical, rect.top());
  float bot = 0.0f;
  // The main interval is the confidence level the user inputted
  float estimate = m_statistic->estimate();
  float mainThreshold = m_statistic->threshold();
  int mainThresholdIndex = Interval::MainDisplayedIntervalThresholdIndex(mainThreshold);
  // Draw each intervals
  for (int i = 0; i < Interval::k_numberOfDisplayedIntervals; i++) {
    float verticalPosition = top - (top - bot) * (i + 1) / (Interval::k_numberOfDisplayedIntervals + 1);
    float threshold = Interval::DisplayedIntervalThresholdAtIndex(mainThreshold, i);
    bool isMainInterval = (i == mainThresholdIndex);
    // Temporarily set the interval to compute the margin of error
    m_statistic->setThreshold(threshold);
    m_statistic->computeInterval();
    float marginOfError = m_statistic->marginOfError();
    // Draw the threshold value
    constexpr int k_ThresholdBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(k_numberSignificantDigits) + 1; // 1 = strlen("%")
    char buffer[k_ThresholdBufferSize];
    Poincare::Print::customPrintf(
        buffer, k_ThresholdBufferSize, "%*.*ef%%", threshold * 100.0f,
        Poincare::Preferences::PrintFloatMode::Decimal, k_numberSignificantDigits);
    // Compute the threshold label position
    KDPoint labelPosition = positionLabel(
        std::round(floatToPixel(Axis::Horizontal, estimate)),
        std::round(floatToPixel(Axis::Vertical, verticalPosition)) + k_labelGraduationLength,
        KDFont::SmallFont->stringSize(buffer), RelativePosition::None, RelativePosition::After);
    KDColor textColor = isMainInterval ? KDColorBlack : Escher::Palette::GrayDarkMiddle;
    ctx->drawString(buffer, labelPosition, KDFont::SmallFont, textColor, k_backgroundColor);

    // Draw the interval segment : --------
    double intervalRightBound = estimate - marginOfError;
    double intervalLeftBound = estimate + marginOfError;
    KDColor intervalColor = isMainInterval ? Escher::Palette::YellowDark : Escher::Palette::GrayDarkMiddle;
    KDCoordinate intervalThickness = isMainInterval ? k_mainIntervalThickness : k_intervalThickness;
    drawHorizontalOrVerticalSegment(
        ctx, rect, Axis::Horizontal, verticalPosition, intervalRightBound,
        intervalLeftBound, intervalColor, intervalThickness);
    // Draw each bounds of the interval : |--------|
    drawGraduationAtPosition(ctx, intervalRightBound, verticalPosition,
                             intervalColor, k_intervalBoundHalfHeight,
                             intervalThickness);
    drawGraduationAtPosition(ctx, intervalLeftBound, verticalPosition,
                             intervalColor, k_intervalBoundHalfHeight,
                             intervalThickness);
  }

  // Restore initial threshold and interval
  m_statistic->setThreshold(mainThreshold);
  m_statistic->computeInterval();

  // Draw label and graduations
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

void StatisticCurveView::drawLabelAndGraduationAtPosition(KDContext * ctx, float position, const char * text, RelativePosition horizontalPosition) const {
  // Draw only if visible
  if ((curveViewRange()->xMin() <= position) && (position <= curveViewRange()->xMax())) {
    float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
    KDCoordinate graduationPosition = drawGraduationAtPosition(ctx, position);

    // Label
    KDPoint labelPosition = positionLabel(graduationPosition,
                                          verticalOrigin,
                                          KDFont::SmallFont->stringSize(text),
                                          horizontalPosition,
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
  float lowerBound = m_statistic->estimate() - m_statistic->marginOfError();
  float upperBound = m_statistic->estimate() + m_statistic->marginOfError();
  float spaceBetweenBounds = floatToPixel(Axis::Horizontal, upperBound) - floatToPixel(Axis::Horizontal, lowerBound);
  // Align labels left and right if they would overlap
  bool realignLabels = spaceBetweenBounds <= k_labelBufferMaxGlyphLength * KDFont::SmallFont->glyphSize().width();
  char buffer[k_labelBufferMaxSize];
  convertFloatToText(lowerBound, buffer, k_labelBufferMaxSize);
  drawLabelAndGraduationAtPosition(ctx, lowerBound, buffer, realignLabels ? RelativePosition::Before : RelativePosition::None);
  convertFloatToText(upperBound, buffer, k_labelBufferMaxSize);
  drawLabelAndGraduationAtPosition(ctx, upperBound, buffer, realignLabels ? RelativePosition::After : RelativePosition::None);
}

KDCoordinate StatisticCurveView::drawGraduationAtPosition(KDContext * ctx, float horizontalPosition, float verticalPosition, KDColor color, KDCoordinate halfHeight, KDCoordinate axisThickness) const {
  float verticalOrigin = std::round(floatToPixel(Axis::Vertical, verticalPosition));
  KDCoordinate graduationPosition = std::round(floatToPixel(Axis::Horizontal, horizontalPosition));
  // Graduation
  KDRect graduation = KDRect(graduationPosition,
                             verticalOrigin - halfHeight,
                             axisThickness,
                             halfHeight * 2 + axisThickness );
  ctx->fillRect(graduation, color);
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
