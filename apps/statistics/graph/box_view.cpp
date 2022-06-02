#include "box_view.h"
#include "box_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;
using namespace Escher;

namespace Statistics {

BoxView::BoxView(Store * store, int series, int * selectedBoxCalculation) :
  CurveView(&m_boxRange, nullptr, nullptr, nullptr),
  m_store(store),
  m_boxRange(BoxRange(store)),
  m_series(series),
  m_selectedBoxCalculation(selectedBoxCalculation)
{
}

bool BoxView::canIncrementSelectedCalculation(int deltaIndex) const {
  assert(deltaIndex != 0);
  return *m_selectedBoxCalculation + deltaIndex >= 0 && *m_selectedBoxCalculation + deltaIndex < m_store->numberOfBoxPlotCalculations(m_series);
}

void BoxView::incrementSelectedCalculation(int deltaIndex) {
  assert(canIncrementSelectedCalculation(deltaIndex));
  *m_selectedBoxCalculation += deltaIndex;
}

KDRect BoxView::selectedCalculationRect() const {
  float calculation = m_store->boxPlotCalculationAtIndex(m_series, *m_selectedBoxCalculation);
  KDCoordinate minX = std::round(floatToPixel(Axis::Horizontal, calculation)) - k_leftSideSize;
  KDCoordinate width = k_leftSideSize + k_rightSideSize;
  // Transpose the rect into parent's view coordinates
  return KDRect(minX, 0, width, BoxFrameHeight(m_store->numberOfValidSeries(Shared::DoublePairStore::DefaultValidSeries))).translatedBy(m_frame.origin());
}

KDRect BoxView::rectToReload() {
  // Transpose the rect into parent's view coordinates
  return boxRect().translatedBy(m_frame.origin());
}

void BoxView::reload(bool resetInterrupted, bool force) {
  CurveView::reload(resetInterrupted, force);
  markRectAsDirty(boxRect());
}

KDRect BoxView::boxRect() const {
  KDCoordinate minX = std::round(floatToPixel(Axis::Horizontal, m_store->minValue(m_series))) - k_leftSideSize;
  KDCoordinate maxX = std::round(floatToPixel(Axis::Horizontal, m_store->maxValue(m_series))) + k_rightSideSize;
  return KDRect(minX, 0, maxX - minX, BoxFrameHeight(m_store->numberOfValidSeries(Shared::DoublePairStore::DefaultValidSeries)));
}

void BoxView::drawRect(KDContext * ctx, KDRect rect) const {
  int numberOfSeries = m_store->numberOfValidSeries(Shared::DoublePairStore::DefaultValidSeries);
  assert(bounds().height() == BoxFrameHeight(numberOfSeries));
  KDColor color = isMainViewSelected() ? DoublePairStore::colorLightOfSeriesAtIndex(m_series) : Palette::GrayWhite;

  // Draw the main box
  double firstQuart = m_store->firstQuartile(m_series);
  double thirdQuart = m_store->thirdQuartile(m_series);
  KDCoordinate firstQuartilePixels = std::round(floatToPixel(Axis::Horizontal, firstQuart));
  KDCoordinate thirdQuartilePixels = std::round(floatToPixel(Axis::Horizontal, thirdQuart));
  ctx->fillRect(KDRect(firstQuartilePixels, k_verticalSideSize, thirdQuartilePixels - firstQuartilePixels, BoxHeight(numberOfSeries)), color);

  // Draw the horizontal lines linking the box to the whiskers
  // Compute the middle from the pixels for a better precision
  float segmentOrd = pixelToFloat(Axis::Vertical, (k_verticalSideSize + BoxHeight(numberOfSeries) + k_verticalSideSize) / 2);
  double lowerWhisker = m_store->lowerWhisker(m_series);
  double upperWhisker = m_store->upperWhisker(m_series);
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Horizontal, segmentOrd, lowerWhisker, firstQuart, color);
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Horizontal, segmentOrd, thirdQuart, upperWhisker, color);

  float lowBound = pixelToFloat(Axis::Vertical, k_verticalSideSize + BoxHeight(numberOfSeries));
  float upBound = pixelToFloat(Axis::Vertical, k_verticalSideSize);

  // Draw each unselected calculations
  int myNumberOfBoxPlotCalculations = m_store->numberOfBoxPlotCalculations(m_series);
  for (int i = 0; i < myNumberOfBoxPlotCalculations; i++) {
    KDColor calculationColor = k_unfocusedColor;
    if (isMainViewSelected()) {
      if (i == *m_selectedBoxCalculation) {
        continue;
      }
      calculationColor = DoublePairStore::colorOfSeriesAtIndex(m_series);
    }
    drawCalculation(ctx, rect, i, lowBound, upBound, segmentOrd, calculationColor, false);
  }
  // Draw the selected calculation afterward, preventing it being overwritten.
  if (isMainViewSelected()) {
    drawCalculation(ctx, rect, *m_selectedBoxCalculation, lowBound, upBound, segmentOrd, k_selectedColor, true);
  }
}

void BoxView::drawCalculation(KDContext * ctx, KDRect rect, int selectedCalculation, float lowBound, float upBound, float segmentOrd, KDColor color, bool isSelected) const {
  assert(selectedCalculation >= 0 && selectedCalculation < m_store->numberOfBoxPlotCalculations(m_series));
  float calculation = m_store->boxPlotCalculationAtIndex(m_series, selectedCalculation);
  if (m_store->boxPlotCalculationIsOutlier(m_series, selectedCalculation)) {
    drawOutlier(ctx, rect, calculation, segmentOrd, color, isSelected);
  } else {
    drawBar(ctx, rect, calculation, lowBound, upBound, color, isSelected);
  }
}

void BoxView::drawBar(KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound, KDColor color, bool isSelected) const {
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, calculation, lowBound, upBound, color, k_quantileBarWidth);
  if (isSelected) {
    lowBound = pixelToFloat(Axis::Vertical, k_verticalSideSize + BoxHeight(m_store->numberOfValidSeries(Shared::DoublePairStore::DefaultValidSeries)) + k_chevronMargin - 1);
    upBound = pixelToFloat(Axis::Vertical, k_verticalSideSize - k_chevronMargin);
    drawChevronSelection(ctx, rect, calculation, lowBound, upBound);
  }
}

void BoxView::drawOutlier(KDContext * ctx, KDRect rect, float calculation, float segmentOrd, KDColor color, bool isSelected) const {
  drawDot(ctx, rect, calculation, segmentOrd, color, k_outlierDotSize);
  if (isSelected) {
    KDCoordinate segmentOrdPixel = floatToPixel(Axis::Vertical, segmentOrd);
    float lowBound = pixelToFloat(Axis::Vertical, segmentOrdPixel + (k_outlierSize + 1)/2 + k_chevronMargin - 1);
    float upBound = pixelToFloat(Axis::Vertical, segmentOrdPixel - k_outlierSize/2 - k_chevronMargin);
    drawChevronSelection(ctx, rect, calculation, lowBound, upBound);
  }
}

void BoxView::drawChevronSelection(KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound) const {
  drawChevron(ctx, rect, calculation, lowBound, k_selectedColor, true);
  drawChevron(ctx, rect, calculation, upBound, k_selectedColor, false);
}

void BoxView::drawChevron(KDContext * ctx, KDRect rect, float x, float y, KDColor color, bool up) const {
  // Place the chevron so that it points two pixels, the left one being (x, y).
  KDCoordinate px = std::round(floatToPixel(Axis::Horizontal, x));
  KDCoordinate py = std::round(floatToPixel(Axis::Vertical, y));
  px += 1 - Chevrons::k_chevronWidth/2;
  py += (up ? 1 : -Chevrons::k_chevronHeight);
  KDRect dotRect(px, py, Chevrons::k_chevronWidth, Chevrons::k_chevronHeight);
  if (!rect.intersects(dotRect)) {
    return;
  }
  KDColor workingBuffer[Chevrons::k_chevronHeight*Chevrons::k_chevronWidth];
  const uint8_t * mask = (const uint8_t *)(up ? Chevrons::UpChevronMask : Chevrons::DownChevronMask);
  ctx->blendRectWithMask(dotRect, color, mask, workingBuffer);
}

}
