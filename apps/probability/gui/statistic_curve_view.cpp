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

float StatisticCurveView::LargestMarginOfError(Statistic * statistic) {
  /* Temporarily sets the statistic's threshold to the largest displayed
   * interval to compute the margin of error needed to display all intervals. */
  double previousThreshold = statistic->threshold();
  float intervalTemp = IntervalThresholdAtIndex(previousThreshold, k_numberOfIntervals - 1);
  statistic->setThreshold(intervalTemp);
  statistic->computeInterval();
  double marginOfError = statistic->marginOfError();
  // Restore the statistic
  statistic->setThreshold(previousThreshold);
  statistic->computeInterval();
  return marginOfError;
}

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

// Return the displayed position of the main threshold interval
int StatisticCurveView::IntervalMainThresholdIndex(float mainThreshold) {
  constexpr float k_thresholdLimits[k_numberOfIntervals - 1] = {0.1f, 0.2f, 0.99f};
  // If mainThreshold is in ]0.2, 0.99], it is the third displayed interval
  for (int index = 0; index < k_numberOfIntervals - 1; index++) {
    if (mainThreshold <= k_thresholdLimits[index]) {
      return index;
    }
  }
  return k_numberOfIntervals - 1;
}

// Return the next threshold interval to display going up or down
float StatisticCurveView::NextThreshold(float threshold, bool up) {
  // There are significant interval threshold to display
  constexpr size_t k_numberOfSignificantThresholds = 14;
  constexpr float k_significantThresholds[k_numberOfSignificantThresholds] = {
      0.1f, 0.2f,  0.3f, 0.4f,  0.5f,  0.6f,  0.7f,
      0.8f, 0.85f, 0.9f, 0.95f, 0.98f, 0.99f, 0.999f};
  assert((up && threshold <= 0.99f) || (!up && threshold > 0.1f));
  size_t start = up ? 0 : 1;
  size_t end = k_numberOfSignificantThresholds + start - 1;
  // If threshold is in ]0.2, 0.3], display 0.4 if going up and 0.2 otherwise
  for (size_t i = start; i < end; i++) {
    if (threshold <= k_significantThresholds[i] || i == end - 1) {
      assert(threshold <= k_significantThresholds[i] || !up);
      size_t nextIndex = up ? i + 1 : i - 1;
      assert(nextIndex >= 0 && nextIndex < k_numberOfSignificantThresholds);
      return k_significantThresholds[nextIndex];
    }
  }
  assert(false);
  return NAN;
}

// Return the interval threshold to display at given index
float StatisticCurveView::IntervalThresholdAtIndex(float threshold, int index) {
  int mainIndex = IntervalMainThresholdIndex(threshold);
  bool up = index > mainIndex;
  int interations = up ? index - mainIndex : mainIndex - index;
  for (int i = 0; i < interations; i++) {
    threshold = NextThreshold(threshold, up);
  }
  return threshold;
}

// Draw the main interval along side with 3 other significant intervals.
void StatisticCurveView::drawInterval(KDContext * ctx, KDRect rect) const {
  /* Distribute the k_numberOfIntervals intervals between top of rect and axis:
   *  i   isMainInterval
   *                                             10%
   *  0       false                              ▔▔▔
   *                                             20%
   *  1       false                          ▔▔▔▔▔▔▔▔▔▔▔
   *                                     |      25.4%      |
   *  2       true                       |▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔|
   *                                             40%
   *  3       false                 ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔
   *
   *  axis                   ------------|--------|--------|-------------------
   *  */
  float top = pixelToFloat(Axis::Vertical, rect.top());
  float bot = 0.0f;
  // The main interval is the confidence level the user inputted
  float estimate = m_statistic->estimate();
  float mainThreshold = m_statistic->threshold();
  int mainThresholdIndex = IntervalMainThresholdIndex(mainThreshold);
  // Draw each intervals
  for (int i = 0; i < k_numberOfIntervals; i++) {
    float verticalPosition = top - (top - bot) * (i + 1) / (k_numberOfIntervals + 1);
    float threshold = IntervalThresholdAtIndex(mainThreshold, i);
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
    drawHorizontalOrVerticalSegment(
        ctx, rect, Axis::Horizontal, verticalPosition, intervalRightBound,
        intervalLeftBound, intervalColor, k_intervalThickness);
    if (isMainInterval) {
      // Draw each bounds of the main interval : |--------|
      drawGraduationAtPosition(ctx, intervalRightBound, verticalPosition,
                               intervalColor, k_mainIntervalBoundHeight,
                               k_intervalThickness);
      drawGraduationAtPosition(ctx, intervalLeftBound, verticalPosition,
                               intervalColor, k_mainIntervalBoundHeight,
                               k_intervalThickness);
    }
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

KDCoordinate StatisticCurveView::drawGraduationAtPosition(KDContext * ctx, float horizontalPosition, float verticalPosition, KDColor color, KDCoordinate length, KDCoordinate axisThickness) const {
  float verticalOrigin = std::round(floatToPixel(Axis::Vertical, verticalPosition));
  KDCoordinate graduationPosition = std::round(floatToPixel(Axis::Horizontal, horizontalPosition));
  // Graduation
  KDRect graduation = KDRect(graduationPosition,
                             verticalOrigin - (length - axisThickness) / 2,
                             1,
                             length);
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
