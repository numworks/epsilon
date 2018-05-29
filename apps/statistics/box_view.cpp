#include "box_view.h"
#include "box_controller.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Statistics {

BoxView::BoxView(BoxController * controller, Store * store, int series, Shared::BannerView * bannerView, Quantile * selectedQuantile, KDColor color, KDColor lightColor) :
  CurveView(&m_boxRange, nullptr, bannerView, nullptr),
  m_store(store),
  m_boxController(controller),
  m_boxRange(BoxRange(store)),
  m_series(series),
  m_selectedQuantile(selectedQuantile),
  m_selectedHistogramColor(color),
  m_selectedHistogramLightColor(lightColor)
{
}

bool BoxView::selectQuantile(int selectedQuantile) {
  if (selectedQuantile < 0 || selectedQuantile > 4) {
    return false;
  }
  if ((int)*m_selectedQuantile != selectedQuantile) {
    reload();
    *m_selectedQuantile = (Quantile)selectedQuantile;
    reload();
  }
  return true;
}

void BoxView::reload() {
  CurveView::reload();
  markRectAsDirty(bounds());
}

void BoxView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);

  KDCoordinate lowBoundPixel = boxLowerBoundPixel();
  KDCoordinate upBoundPixel = boxUpperBoundPixel();
  float lowBound = pixelToFloat(Axis::Vertical, upBoundPixel);
  float upBound = pixelToFloat(Axis::Vertical, lowBoundPixel);
  double minVal = m_store->minValue(m_series);
  double firstQuart = m_store->firstQuartile(m_series);
  double thirdQuart = m_store->thirdQuartile(m_series);
  double maxVal = m_store->maxValue(m_series);

  bool isSelected = m_boxController->selectedSeries() == m_series;
  KDColor boxColor = isSelected ? m_selectedHistogramLightColor : Palette::GreyWhite;
  // Draw the main box
  KDCoordinate firstQuartilePixels = std::round(floatToPixel(Axis::Horizontal, firstQuart));
  KDCoordinate thirdQuartilePixels = std::round(floatToPixel(Axis::Horizontal, thirdQuart));
  ctx->fillRect(KDRect(firstQuartilePixels, lowBoundPixel, thirdQuartilePixels - firstQuartilePixels+2,
    upBoundPixel-lowBoundPixel), boxColor);

  // Draw the horizontal lines linking the box to the extreme bounds
  KDColor horizontalColor = isSelected ? m_selectedHistogramColor : Palette::GreyDark;
  float segmentOrd = (lowBound + upBound)/ 2.0f;
  drawSegment(ctx, rect, Axis::Horizontal, segmentOrd, minVal, firstQuart, horizontalColor);
  drawSegment(ctx, rect, Axis::Horizontal, segmentOrd, thirdQuart, maxVal, horizontalColor);

  double calculations[5] = {minVal, firstQuart, m_store->median(m_series), thirdQuart, maxVal};
  /* We then draw all the vertical lines of the box and then recolor the
   * the selected quantile (if there is one). As two quantiles can have the same
   * value, we cannot choose line colors and then color only once the vertical
   * lines. This solution could hide the highlighted line by coloring the next
   * quantile if it has the same value. */
  for (int k = 0; k < 5; k++) {
    drawSegment(ctx, rect, Axis::Vertical, calculations[k], lowBound, upBound, Palette::GreyMiddle, 2);
  }
  if (isMainViewSelected()) {
    drawSegment(ctx, rect, Axis::Vertical, calculations[(int)*m_selectedQuantile], lowBound, upBound, Palette::YellowDark, 2);
  }
}

KDCoordinate BoxView::boxLowerBoundPixel() const {
  return bounds().height() / 2 - k_boxHeight / 2;
}

KDCoordinate BoxView::boxUpperBoundPixel() const {
  return bounds().height() / 2 + k_boxHeight / 2;
}


}
