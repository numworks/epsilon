#include "interval_curve_view.h"
#include <poincare/print.h>
#include <cmath>

namespace Inference {

void IntervalCurveView::drawRect(KDContext * ctx, KDRect rect) const {
  StatisticCurveView::drawRect(ctx, rect);
  drawInterval(ctx, rect);
}

void IntervalCurveView::resetSelectedInterval() {
  m_selectedIntervalIndex = Interval::MainDisplayedIntervalThresholdIndex(m_interval->threshold());
}

void IntervalCurveView::selectedIntervalEstimateAndMarginOfError(float * estimate, float * marginOfError) {
  float mainThreshold = m_interval->threshold();
  // Temporarily set the interval to compute the selected one
  m_interval->setThreshold(Interval::DisplayedIntervalThresholdAtIndex(m_interval->threshold(), m_selectedIntervalIndex));
  m_interval->compute();
  *estimate = m_interval->estimate();
  *marginOfError = m_interval->marginOfError();
  // Restore initial threshold and interval
  m_interval->setThreshold(mainThreshold);
  m_interval->compute();
}

// Draw the main interval along side with 3 other significant intervals.
void IntervalCurveView::drawInterval(KDContext * ctx, KDRect rect) const {
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
  float estimate = m_interval->estimate();
  float mainThreshold = m_interval->threshold();
  // Draw each intervals
  for (int i = 0; i < Interval::k_numberOfDisplayedIntervals; i++) {
    float verticalPosition = top - (top - bot) * (i + 1) / (Interval::k_numberOfDisplayedIntervals + 1);
    float threshold = Interval::DisplayedIntervalThresholdAtIndex(mainThreshold, i);
    bool isMainInterval = (i == m_selectedIntervalIndex);
    // Temporarily set the interval to compute the margin of error
    m_interval->setThreshold(threshold);
    m_interval->compute();
    float marginOfError = m_interval->marginOfError();
    // Draw the threshold value
    constexpr int k_ThresholdBufferSize = Poincare::PrintFloat::charSizeForFloatsWithPrecision(k_numberSignificantDigits) + 1; // 1 = strlen("%")
    char buffer[k_ThresholdBufferSize];
    Poincare::Print::CustomPrintf(
        buffer, k_ThresholdBufferSize, "%*.*ef%%", threshold * 100.0f,
        Poincare::Preferences::PrintFloatMode::Decimal, k_numberSignificantDigits);
    // Compute the threshold label position
    KDPoint labelPosition = positionLabel(
        std::round(floatToPixel(Axis::Horizontal, estimate)),
        std::round(floatToPixel(Axis::Vertical, verticalPosition)) + k_labelGraduationLength,
        KDFont::Font(KDFont::Size::Small)->stringSize(buffer), RelativePosition::None, RelativePosition::After);
    KDColor textColor = isMainInterval ? KDColorBlack : Escher::Palette::GrayDarkMiddle;
    if (rect.contains(labelPosition)) {
      ctx->drawString(buffer, labelPosition, KDFont::Size::Small, textColor, k_backgroundColor);
    }

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
    if (isMainInterval) {
      // Draw label and graduations
      drawIntervalLabelAndGraduation(ctx);
    }
  }

  // Restore initial threshold and interval
  m_interval->setThreshold(mainThreshold);
  m_interval->compute();
}

void IntervalCurveView::drawLabelAndGraduationAtPosition(KDContext * ctx, float position, const char * text, RelativePosition horizontalPosition) const {
  // Draw only if visible
  if ((curveViewRange()->xMin() <= position) && (position <= curveViewRange()->xMax())) {
    float verticalOrigin = std::round(floatToPixel(Axis::Vertical, 0.0f));
    KDCoordinate graduationPosition = drawGraduationAtPosition(ctx, position);

    // Label
    KDPoint labelPosition = positionLabel(graduationPosition,
                                          verticalOrigin,
                                          KDFont::Font(KDFont::Size::Small)->stringSize(text),
                                          horizontalPosition,
                                          RelativePosition::Before);
    ctx->drawString(text, labelPosition, KDFont::Size::Small, KDColorBlack, k_backgroundColor);
  }
}

void IntervalCurveView::drawIntervalLabelAndGraduation(KDContext * ctx) const {
  float lowerBound = m_interval->estimate() - m_interval->marginOfError();
  float upperBound = m_interval->estimate() + m_interval->marginOfError();
  float spaceBetweenBounds = floatToPixel(Axis::Horizontal, upperBound) - floatToPixel(Axis::Horizontal, lowerBound);
  // Align labels left and right if they would overlap
  bool realignLabels = spaceBetweenBounds <= k_labelBufferMaxGlyphLength * KDFont::GlyphWidth(KDFont::Size::Small);
  char buffer[k_labelBufferMaxSize];
  convertFloatToText(lowerBound, buffer, k_labelBufferMaxSize);
  drawLabelAndGraduationAtPosition(ctx, lowerBound, buffer, realignLabels ? RelativePosition::Before : RelativePosition::None);
  convertFloatToText(upperBound, buffer, k_labelBufferMaxSize);
  drawLabelAndGraduationAtPosition(ctx, upperBound, buffer, realignLabels ? RelativePosition::After : RelativePosition::None);
}

void IntervalCurveView::convertFloatToText(float value, char * buffer, int bufferSize) const {
  Poincare::PrintFloat::ConvertFloatToText<float>(value,
                                                  buffer,
                                                  k_labelBufferMaxSize,
                                                  k_labelBufferMaxGlyphLength,
                                                  k_numberSignificantDigits,
                                                  Poincare::Preferences::PrintFloatMode::Decimal);
}

}  // namespace Inference
