#include "box_view.h"
#include <assert.h>
#include <math.h>

namespace Statistics {

BoxView::BoxView(Store * store, View * bannerView) :
  CurveView(&m_boxRange, nullptr, bannerView, nullptr, 0.0f, 0.2f, 0.4f, 0.2f),
  m_store(store),
  m_boxRange(BoxRange(store)),
  m_selectedQuantile(Quantile::Min)
{
}

void BoxView::reloadSelection() {
  float calculation = 0.0f;
  switch(m_selectedQuantile) {
    case BoxView::Quantile::Min:
      calculation = m_store->minValue();
      break;
    case BoxView::Quantile::FirstQuartile:
      calculation = m_store->firstQuartile();
      break;
    case BoxView::Quantile::Median:
      calculation = m_store->median();
      break;
    case BoxView::Quantile::ThirdQuartile:
      calculation = m_store->thirdQuartile();
      break;
    case BoxView::Quantile::Max:
      calculation = m_store->maxValue();
      break;
    default:
      break;
  }
  float pixelUpperBound = floatToPixel(Axis::Vertical, 0.2f)+1;
  float pixelLowerBound = floatToPixel(Axis::Vertical, 0.8)-1;
  float selectedValueInPixels = floatToPixel(Axis::Horizontal, calculation);
  KDRect dirtyZone(KDRect(selectedValueInPixels-1, pixelLowerBound, 2, pixelUpperBound - pixelLowerBound));
  markRectAsDirty(dirtyZone);
}

BoxView::Quantile BoxView::selectedQuantile() {
  return m_selectedQuantile;
}

bool BoxView::selectQuantile(int selectedQuantile) {
  if (selectedQuantile < 0 || selectedQuantile > 4) {
    return false;
  }
  if ((int)m_selectedQuantile != selectedQuantile) {
    reloadSelection();
    m_selectedQuantile = (Quantile)selectedQuantile;
    reloadSelection();
  }
  return true;
}

void BoxView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  float calculations[5] = {m_store->minValue(), m_store->firstQuartile(), m_store->median(), m_store->thirdQuartile(), m_store->maxValue()};
  float lowBounds[5] = {0.4f, 0.2f, 0.2f, 0.2f, 0.4f};
  float upBounds[5] = {0.6f, 0.8f, 0.8f, 0.8f, 0.6f};
  /* We first draw all the vertical lines of the box and then recolor the
   * the selected quantile (if there is one). As two quantiles can have the same
   * value, we cannot choose line colors and then color only once the vertical
   * lines. This solution could hide the highlighed line by coloring the next
   * quantile if it has the same value. */
  for (int k = 0; k < 5; k++) {
    drawSegment(ctx, rect, Axis::Vertical, calculations[k], lowBounds[k], upBounds[k], KDColorBlack);
  }
  if (isMainViewSelected()) {
    drawSegment(ctx, rect, Axis::Vertical, calculations[(int)m_selectedQuantile], lowBounds[(int)m_selectedQuantile], upBounds[(int)m_selectedQuantile], KDColorRed);
  }
  drawSegment(ctx, rect, Axis::Horizontal, 0.5f, m_store->minValue(), m_store->firstQuartile(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.5f, m_store->thirdQuartile(), m_store->maxValue(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.2f, m_store->firstQuartile(), m_store->thirdQuartile(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.8f, m_store->firstQuartile(), m_store->thirdQuartile(), KDColorBlack);
}

char * BoxView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

}
