#include "box_view.h"
#include <assert.h>
#include <cmath>

using namespace Shared;

namespace Statistics {

BoxView::BoxView(Store * store, BannerView * bannerView, Quantile * selectedQuantile) :
  CurveView(&m_boxRange, nullptr, bannerView, nullptr),
  m_store(store),
  m_boxRange(BoxRange(store)),
  m_labels{},
  m_selectedQuantile(selectedQuantile)
{
}

void BoxView::reload() {
  CurveView::reload();
  CalculPointer calculationMethods[5] = {&Store::minValue, &Store::firstQuartile, &Store::median, &Store::thirdQuartile,
    &Store::maxValue};
  float calculation = (m_store->*calculationMethods[(int)*m_selectedQuantile])();
  float pixelUpperBound = floatToPixel(Axis::Vertical, 0.2f)+1;
  float pixelLowerBound = floatToPixel(Axis::Vertical, 0.8)-1;
  float selectedValueInPixels = floatToPixel(Axis::Horizontal, calculation)-1;
  KDRect dirtyZone(KDRect(selectedValueInPixels, pixelLowerBound, 4, pixelUpperBound - pixelLowerBound));
  markRectAsDirty(dirtyZone);
}

BoxView::Quantile BoxView::selectedQuantile() {
  return *m_selectedQuantile;
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

void BoxView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  float lowBound = 0.35f;
  float upBound = 0.65f;
  // Draw the main box
  KDCoordinate firstQuartilePixels = std::round(floatToPixel(Axis::Horizontal, m_store->firstQuartile()));
  KDCoordinate thirdQuartilePixels = std::round(floatToPixel(Axis::Horizontal, m_store->thirdQuartile()));
  KDCoordinate lowBoundPixel = std::round(floatToPixel(Axis::Vertical, upBound));
  KDCoordinate upBoundPixel = std::round(floatToPixel(Axis::Vertical, lowBound));
  ctx->fillRect(KDRect(firstQuartilePixels, lowBoundPixel, thirdQuartilePixels - firstQuartilePixels+2,
    upBoundPixel-lowBoundPixel), Palette::GreyWhite);
  // Draw the horizontal lines linking the box to the extreme bounds
  drawSegment(ctx, rect, Axis::Horizontal, 0.5f, m_store->minValue(), m_store->firstQuartile(), Palette::GreyDark);
  drawSegment(ctx, rect, Axis::Horizontal, 0.5f, m_store->thirdQuartile(), m_store->maxValue(), Palette::GreyDark);

  double calculations[5] = {m_store->minValue(), m_store->firstQuartile(), m_store->median(), m_store->thirdQuartile(), m_store->maxValue()};
  /* We then draw all the vertical lines of the box and then recolor the
   * the selected quantile (if there is one). As two quantiles can have the same
   * value, we cannot choose line colors and then color only once the vertical
   * lines. This solution could hide the highlighed line by coloring the next
   * quantile if it has the same value. */
  for (int k = 0; k < 5; k++) {
    drawSegment(ctx, rect, Axis::Vertical, calculations[k], lowBound, upBound, Palette::GreyMiddle, 2);
  }
  if (isMainViewSelected()) {
    drawSegment(ctx, rect, Axis::Vertical, calculations[(int)*m_selectedQuantile], lowBound, upBound, Palette::YellowDark, 2);
  }
}

char * BoxView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

}
