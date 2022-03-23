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

// TODO: Handle and use selectedBoxCalculation beyond 4
bool BoxView::selectQuantile(int selectedBoxCalculation) {
  if (selectedBoxCalculation < 0 || selectedBoxCalculation > 4) {
    return false;
  }
  if (*m_selectedBoxCalculation != selectedBoxCalculation) {
    reloadQuantile();
    *m_selectedBoxCalculation = selectedBoxCalculation;
    reloadQuantile();
  }
  return true;
}

void BoxView::reloadQuantile() {
  CurveView::reload();
  KDCoordinate minY = calculationLowerBoundPixel();
  KDCoordinate maxY = calculationUpperBoundPixel();
  float calculation = calculationAtIndex(*m_selectedBoxCalculation);
  KDCoordinate minX = std::round(floatToPixel(Axis::Horizontal, calculation)) - k_leftMargin;
  KDCoordinate width = k_leftMargin + k_rightMargin;
  KDRect dirtyRect = KDRect(minX, minY, width, maxY - minY);
  markRectAsDirty(dirtyRect);
}

void BoxView::reload(bool resetInterrupted, bool force) {
  CurveView::reload(resetInterrupted, force);
  KDCoordinate minY = calculationLowerBoundPixel();
  KDCoordinate maxY = calculationUpperBoundPixel();
  KDCoordinate minX = std::round(floatToPixel(Axis::Horizontal, m_store->minValue(m_series))) - k_leftMargin;
  KDCoordinate maxX = std::round(floatToPixel(Axis::Horizontal, m_store->maxValue(m_series))) + k_rightMargin;
  KDRect dirtyRect = KDRect(minX, minY, maxX - minX, maxY - minY);
  markRectAsDirty(dirtyRect);
}

void BoxView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);

  KDColor color = isMainViewSelected() ? DoublePairStore::colorLightOfSeriesAtIndex(m_series) : Palette::GrayBright;
  KDCoordinate lowBoundPixel = boxLowerBoundPixel();
  KDCoordinate upBoundPixel = boxUpperBoundPixel();

  // Draw the main box
  double firstQuart = m_store->firstQuartile(m_series);
  double thirdQuart = m_store->thirdQuartile(m_series);
  KDCoordinate firstQuartilePixels = std::round(floatToPixel(Axis::Horizontal, firstQuart));
  KDCoordinate thirdQuartilePixels = std::round(floatToPixel(Axis::Horizontal, thirdQuart));
  ctx->fillRect(KDRect(firstQuartilePixels, lowBoundPixel, thirdQuartilePixels - firstQuartilePixels, upBoundPixel-lowBoundPixel), color);

  // Draw the horizontal lines linking the box to the whiskers
  float lowBound = pixelToFloat(Axis::Vertical, upBoundPixel);
  float upBound = pixelToFloat(Axis::Vertical, lowBoundPixel);
  float segmentOrd = (lowBound + upBound)/ 2.0f;
  double lowerWhisker = m_store->lowerWhisker(m_series);
  double upperWhisker = m_store->upperWhisker(m_series);
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Horizontal, segmentOrd, lowerWhisker, firstQuart, color);
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Horizontal, segmentOrd, thirdQuart, upperWhisker, color);

  // Draw each calculations
  int numberOfLowerOutliers = m_store->numberOfLowerOutliers(m_series);
  // Draw unselected calculations
  for (size_t i = 0; i < numberOfCalculation(); i++) {
    KDColor calculationColor = k_unfocusedColor;
    if (isMainViewSelected()) {
      if (i == *m_selectedBoxCalculation) {
        continue;
      }
      calculationColor = DoublePairStore::colorOfSeriesAtIndex(m_series);
    }
    float calculation = calculationAtIndex(i);
    if (i >= numberOfLowerOutliers && i < numberOfLowerOutliers + k_numberOfNonOutlierCalculations) {
      drawBar(ctx, rect, calculation, lowBound, upBound, calculationColor, false);
    } else {
      drawOutlier(ctx, rect, calculation, segmentOrd, calculationColor, false);
    }
  }
  // Draw the selected calculation afterward, preventing it being overwritten.
  if (isMainViewSelected()) {
    int selectedCalculation = *m_selectedBoxCalculation;
    assert(selectedCalculation >= 0 && selectedCalculation < numberOfCalculation());
    float calculation = calculationAtIndex(selectedCalculation);
    if (selectedCalculation >= numberOfLowerOutliers && selectedCalculation < numberOfLowerOutliers + k_numberOfNonOutlierCalculations) {
      drawBar(ctx, rect, calculation, lowBound, upBound, k_selectedColor, true);
    } else {
      drawOutlier(ctx, rect, calculation, segmentOrd, k_selectedColor, true);
    }
  }
}

// TODO: Move this logic in a controller
float BoxView::calculationAtIndex(int i) const {
  if (i < 0) {
    return m_store->minValue(m_series);
  }
  int numberOfLowerOutliers = m_store->numberOfLowerOutliers(m_series);
  if (i < numberOfLowerOutliers) {
    return m_store->lowerOutlierAtIndex(m_series, i);
  }
  if (i == numberOfLowerOutliers) {
    return m_store->lowerWhisker(m_series);
  }
  if (i == numberOfLowerOutliers + 1) {
    return m_store->firstQuartile(m_series);
  }
  if (i == numberOfLowerOutliers + 2) {
    return m_store->median(m_series);
  }
  if (i == numberOfLowerOutliers + 3) {
    return m_store->thirdQuartile(m_series);
  }
  if (i == numberOfLowerOutliers + 4) {
    return m_store->upperWhisker(m_series);
  }
  int numberOfUpperOutliers = m_store->numberOfUpperOutliers(m_series);
  if (i < numberOfLowerOutliers + k_numberOfNonOutlierCalculations + numberOfUpperOutliers) {
    return m_store->upperOutlierAtIndex(m_series, i - numberOfLowerOutliers - k_numberOfNonOutlierCalculations);
  }
  return m_store->maxValue(m_series);
}

int BoxView::numberOfCalculation() const {
  // Outliers + Lower/Upper Whisker + First/Third Quartile + Median
  return m_store->numberOfLowerOutliers(m_series) + k_numberOfNonOutlierCalculations + m_store->numberOfUpperOutliers(m_series);
}

void BoxView::drawBar(KDContext * ctx, KDRect rect, float calculation, float lowBound, float upBound, KDColor color, bool isSelected) const {
  drawHorizontalOrVerticalSegment(ctx, rect, Axis::Vertical, calculation, lowBound, upBound, color, k_quantileBarWidth);
  if (isSelected) {
    lowBound = pixelToFloat(Axis::Vertical, boxUpperBoundPixel() + k_chevronMargin - 1);
    upBound = pixelToFloat(Axis::Vertical, boxLowerBoundPixel() - k_chevronMargin);
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

KDCoordinate BoxView::boxLowerBoundPixel() const {
  return bounds().height() / 2 - boxHeight() / 2;
}

KDCoordinate BoxView::boxUpperBoundPixel() const {
  // Ceil boxHeight to handle odd boxHeight
  return bounds().height() / 2 + (boxHeight() + 1) / 2;
}

KDCoordinate BoxView::calculationLowerBoundPixel() const {
  return boxLowerBoundPixel() - k_verticalMargin;
}

KDCoordinate BoxView::calculationUpperBoundPixel() const {
  return boxUpperBoundPixel() + k_verticalMargin;
}

}
