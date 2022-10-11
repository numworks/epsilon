#include "interval_curve_view.h"
#include <poincare/print.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Inference {

// IntervalAxis

static void convertFloatToText(float f, char * buffer, int bufferSize) {
  Poincare::PrintFloat::ConvertFloatToText<float>(f, buffer, IntervalAxis::k_bufferSize, IntervalAxis::k_glyphLength, PlotPolicy::AbstractLabeledAxis::k_numberSignificantDigits, Poincare::Preferences::PrintFloatMode::Decimal);
}

void IntervalAxis::reloadAxis(AbstractPlotView * plotView, AbstractPlotView::Axis axis) {
  Interval * plotInterval = interval(plotView);
  float low = plotInterval->estimate() - plotInterval->marginOfError();
  float high = plotInterval->estimate() + plotInterval->marginOfError();
  float spaceBetweenBounds = plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, high) - plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, low);
  m_realignLabels = spaceBetweenBounds <= k_glyphLength * KDFont::GlyphWidth(AbstractPlotView::k_font);

  convertFloatToText(low, m_labels[0], k_bufferSize);
  convertFloatToText(high, m_labels[1], k_bufferSize);
  /* Memoize the interval bounds, as at the time of display, there is no guarantee the bounds will be correct. */
  m_ticks[0] = low;
  m_ticks[1] = high;
}

float IntervalAxis::tickPosition(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis) const {
  if (i > k_numberOfLabels) {
    return NAN;
  }
  return m_ticks[i];
}

void IntervalAxis::drawLabel(int i, float t, const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis) const {
  AbstractPlotView::RelativePosition yRelative = AbstractPlotView::RelativePosition::After;
  AbstractPlotView::RelativePosition xRelative = m_realignLabels ? (i == 0 ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::After) : AbstractPlotView::RelativePosition::There;
  plotView->drawLabel(ctx, rect, m_labels[i], Coordinate2D<float>(t, 0.f), xRelative, yRelative, KDColorBlack);
}

// IntervalPlotPolicy

void IntervalPlotPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
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

  float graduationTopLength = plotView->pixelHeight() * k_intervalBoundHalfHeight;
  float top = plotView->pixelToFloat(AbstractPlotView::Axis::Vertical, rect.top());
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
    constexpr int k_ThresholdBufferSize = PrintFloat::charSizeForFloatsWithPrecision(PlotPolicy::AbstractLabeledAxis::k_numberSignificantDigits) + 1; // 1 = strlen("%")
    char buffer[k_ThresholdBufferSize];
    Print::CustomPrintf(buffer, k_ThresholdBufferSize, "%*.*ef%%", threshold * 100.0f, Preferences::PrintFloatMode::Decimal, PlotPolicy::AbstractLabeledAxis::k_numberSignificantDigits);
    KDColor textColor = isMainInterval ? KDColorBlack : Escher::Palette::GrayDarkMiddle;
    plotView->drawLabel(ctx, rect, buffer, Coordinate2D<float>(estimate, verticalPosition), AbstractPlotView::RelativePosition::There, AbstractPlotView::RelativePosition::Before, textColor, true);

    // Draw the interval segment : --------
    double intervalRightBound = estimate - marginOfError;
    double intervalLeftBound = estimate + marginOfError;
    KDColor intervalColor = isMainInterval ? Escher::Palette::YellowDark : Escher::Palette::GrayDarkMiddle;
    KDCoordinate intervalThickness = isMainInterval ? k_mainIntervalThickness : k_intervalThickness;
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Horizontal, verticalPosition, intervalRightBound, intervalLeftBound, intervalColor, intervalThickness);
    // Draw each bounds of the interval : |--------|
    float graduationBottomLength = plotView->pixelHeight() * (k_intervalBoundHalfHeight + intervalThickness);
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, intervalRightBound, verticalPosition + graduationTopLength, verticalPosition - graduationBottomLength, intervalColor, intervalThickness);
    plotView->drawStraightSegment(ctx, rect, AbstractPlotView::Axis::Vertical, intervalLeftBound, verticalPosition + graduationTopLength, verticalPosition - graduationBottomLength, intervalColor, intervalThickness);
  }

  // Restore initial threshold and interval
  m_interval->setThreshold(mainThreshold);
  m_interval->compute();
}

// IntervalCurveView

IntervalCurveView::IntervalCurveView(Interval * interval) :
  PlotView(interval)
{
  // IntervalPlotPolicy
  m_interval = interval;
}

void IntervalCurveView::reload(bool resetInterruption, bool force) {
  /* Set the interval so that the axis displays the right bounds. */
  float mainThreshold = m_interval->threshold();
  m_interval->setThreshold(Interval::DisplayedIntervalThresholdAtIndex(m_interval->threshold(), m_selectedIntervalIndex));
  m_interval->compute();
  AbstractPlotView::reload(resetInterruption, force);
  m_interval->setThreshold(mainThreshold);
  m_interval->compute();
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

}  // namespace Inference
