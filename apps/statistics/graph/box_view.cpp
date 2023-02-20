#include "box_view.h"
#include "box_controller.h"
#include <assert.h>
#include <cmath>

using namespace Escher;
using namespace Poincare;
using namespace Shared;

namespace Statistics {

// BoxPlotPolicy

typedef AbstractPlotView::Axis Axis;

void BoxPlotPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  int numberOfSeries = m_store->numberOfActiveSeries(Shared::DoublePairStore::DefaultActiveSeriesTest);
  assert(plotView->bounds().height() == BoxFrameHeight(numberOfSeries));
  KDColor color = plotView->hasFocus() ? DoublePairStore::colorLightOfSeriesAtIndex(m_series) : Palette::GrayWhite;

  // Draw the main box
  double firstQuart = m_store->firstQuartile(m_series);
  double thirdQuart = m_store->thirdQuartile(m_series);
  KDCoordinate firstQuartilePixels = plotView->floatToKDCoordinatePixel(Axis::Horizontal, firstQuart);
  KDCoordinate thirdQuartilePixels = plotView->floatToKDCoordinatePixel(Axis::Horizontal, thirdQuart);
  ctx->fillRect(KDRect(firstQuartilePixels, k_verticalSideSize, thirdQuartilePixels - firstQuartilePixels, BoxHeight(numberOfSeries)), color);

  // Draw the horizontal lines linking the box to the whiskers
  // Compute the middle from the pixels for a better precision
  float segmentOrd = plotView->pixelToFloat(Axis::Vertical, (k_verticalSideSize + BoxHeight(numberOfSeries) + k_verticalSideSize) / 2);
  double lowerWhisker = m_store->lowerWhisker(m_series);
  double upperWhisker = m_store->upperWhisker(m_series);
  plotView->drawStraightSegment(ctx, rect, Axis::Horizontal, segmentOrd, lowerWhisker, firstQuart, color);
  plotView->drawStraightSegment(ctx, rect, Axis::Horizontal, segmentOrd, thirdQuart, upperWhisker, color);

  float lowBound = plotView->pixelToFloat(Axis::Vertical, k_verticalSideSize + BoxHeight(numberOfSeries));
  float upBound = plotView->pixelToFloat(Axis::Vertical, k_verticalSideSize);

  // Draw each unselected calculations
  const int myNumberOfBoxPlotCalculations = m_store->numberOfBoxPlotCalculations(m_series);
  for (int i = 0; i < myNumberOfBoxPlotCalculations; i++) {
    KDColor calculationColor = k_unfocusedColor;
    if (plotView->hasFocus()) {
      if (i == *m_selectedBoxCalculation) {
        continue;
      }
      calculationColor = DoublePairStore::colorOfSeriesAtIndex(m_series);
    }
    drawCalculation(plotView, ctx, rect, i, lowBound, upBound, segmentOrd, calculationColor, false);
  }
  // Draw the selected calculation afterward, preventing it being overwritten.
  if (plotView->hasFocus()) {
    drawCalculation(plotView, ctx, rect, *m_selectedBoxCalculation, lowBound, upBound, segmentOrd, k_selectedColor, true);
  }
}

void BoxPlotPolicy::drawCalculation(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, int selectedCalculation, float lowBound, float upBound, float segmentOrd, KDColor color, bool isSelected) const {
  assert(selectedCalculation >= 0 && selectedCalculation < m_store->numberOfBoxPlotCalculations(m_series));
  float calculation = m_store->boxPlotCalculationAtIndex(m_series, selectedCalculation);
  if (m_store->boxPlotCalculationIsOutlier(m_series, selectedCalculation)) {
    drawOutlier(plotView, ctx, rect, calculation, segmentOrd, color, isSelected);
  } else {
    drawBar(plotView, ctx, rect, calculation, lowBound, upBound, color, isSelected);
  }
}

void BoxPlotPolicy::drawBar(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound, KDColor color, bool isSelected) const {
  plotView->drawStraightSegment(ctx, rect, Axis::Vertical, calculation, lowBound, upBound, color, k_quantileBarWidth);
  if (isSelected) {
    lowBound = plotView->pixelToFloat(Axis::Vertical, k_verticalSideSize + BoxHeight(m_store->numberOfActiveSeries(Shared::DoublePairStore::DefaultActiveSeriesTest)) + k_chevronMargin - 1);
    upBound = plotView->pixelToFloat(Axis::Vertical, k_verticalSideSize - k_chevronMargin);
    drawChevronSelection(plotView, ctx, rect, calculation, lowBound, upBound);
  }
}

void BoxPlotPolicy::drawOutlier(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float calculation, float segmentOrd, KDColor color, bool isSelected) const {
  plotView->drawDot(ctx, rect, k_outlierDotSize, Coordinate2D<float>(calculation, segmentOrd), color);
  if (isSelected) {
    KDCoordinate segmentOrdPixel = plotView->floatToKDCoordinatePixel(Axis::Vertical, segmentOrd);
    float lowBound = plotView->pixelToFloat(Axis::Vertical, segmentOrdPixel + (k_outlierSize + 1)/2 + k_chevronMargin - 1);
    float upBound = plotView->pixelToFloat(Axis::Vertical, segmentOrdPixel - k_outlierSize/2 - k_chevronMargin);
    drawChevronSelection(plotView, ctx, rect, calculation, lowBound, upBound);
  }
}

void BoxPlotPolicy::drawChevronSelection(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound) const {
  drawChevron(plotView, ctx, rect, calculation, lowBound, k_selectedColor, OMG::NewDirection::Up());
  drawChevron(plotView, ctx, rect, calculation, upBound, k_selectedColor, OMG::NewDirection::Down());
}

void BoxPlotPolicy::drawChevron(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float x, float y, KDColor color, OMG::NewVerticalDirection direction) const {
  // Place the chevron so that it points two pixels, the left one being (x, y).
  KDCoordinate px = plotView->floatToKDCoordinatePixel(Axis::Horizontal, x);
  KDCoordinate py = plotView->floatToKDCoordinatePixel(Axis::Vertical, y);
  px += 1 - Chevrons::k_chevronWidth/2;
  py += (direction.isUp() ? 1 : -Chevrons::k_chevronHeight);
  KDRect dotRect(px, py, Chevrons::k_chevronWidth, Chevrons::k_chevronHeight);
  if (!rect.intersects(dotRect)) {
    return;
  }
  KDColor workingBuffer[Chevrons::k_chevronHeight*Chevrons::k_chevronWidth];
  const uint8_t * mask = (const uint8_t *)(direction.isUp() ? Chevrons::UpChevronMask : Chevrons::DownChevronMask);
  ctx->blendRectWithMask(dotRect, color, mask, workingBuffer);
}

// BoxView

BoxView::BoxView(Store * store, int series, int * selectedBoxCalculation) :
  PlotView(&m_boxRange),
  m_boxRange(store)
{
  // BoxPlotPolicy
  m_store = store;
  m_series = series;
  m_selectedBoxCalculation = selectedBoxCalculation;
}

void BoxView::reload(bool resetInterruption, bool force) {
  AbstractPlotView::reload(resetInterruption, force);
  markRectAsDirty(boxRect());
}

KDRect BoxView::selectedCalculationRect() const {
  float calculation = m_store->boxPlotCalculationAtIndex(m_series, *m_selectedBoxCalculation);
  KDCoordinate minX = floatToKDCoordinatePixel(Axis::Horizontal, calculation) - k_leftSideSize;
  KDCoordinate width = k_leftSideSize + k_rightSideSize;
  // Transpose the rect into parent's view coordinates
  return KDRect(minX, 0, width, BoxFrameHeight(m_store->numberOfActiveSeries(Shared::DoublePairStore::DefaultActiveSeriesTest))).translatedBy(m_frame.origin());
}

bool BoxView::canIncrementSelectedCalculation(int deltaIndex) const {
  assert(deltaIndex != 0);
  return *m_selectedBoxCalculation + deltaIndex >= 0 && *m_selectedBoxCalculation + deltaIndex < m_store->numberOfBoxPlotCalculations(m_series);
}

void BoxView::incrementSelectedCalculation(int deltaIndex) {
  assert(canIncrementSelectedCalculation(deltaIndex));
  *m_selectedBoxCalculation += deltaIndex;
}

KDRect BoxView::rectToReload() {
  // Transpose the rect into parent's view coordinates
  return boxRect().translatedBy(m_frame.origin());
}

KDRect BoxView::boxRect() const {
  KDCoordinate minX = floatToKDCoordinatePixel(Axis::Horizontal, m_store->minValue(m_series)) - k_leftSideSize;
  KDCoordinate maxX = floatToKDCoordinatePixel(Axis::Horizontal, m_store->maxValue(m_series)) + k_rightSideSize;
  return KDRect(minX, 0, maxX - minX, BoxFrameHeight(m_store->numberOfActiveSeries(Shared::DoublePairStore::DefaultActiveSeriesTest)));
}

}
