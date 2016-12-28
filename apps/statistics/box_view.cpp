#include "box_view.h"
#include <assert.h>
#include <math.h>

namespace Statistics {

BoxView::BoxView(Data * data) :
  CurveViewWithBanner(&m_boxWindow, 0.0f, 0.2f, 0.4f, 0.2f),
  m_data(data),
  m_boxWindow(BoxWindow(data)),
  m_selectedQuantile(0),
  m_bannerView(BoxBannerView(data, this))
{
}

void BoxView::reloadSelection() {
  float calculations[5] = {m_data->minValue(), m_data->firstQuartile(), m_data->median(), m_data->thirdQuartile(), m_data->maxValue()};
  float pixelUpperBound = floatToPixel(Axis::Vertical, 0.2f)+1;
  float pixelLowerBound = floatToPixel(Axis::Vertical, 0.8)-1;
  float selectedValueInPixels = floatToPixel(Axis::Horizontal, calculations[m_selectedQuantile]);
  KDRect dirtyZone(KDRect(selectedValueInPixels-1, pixelLowerBound, 2, pixelUpperBound - pixelLowerBound));
  markRectAsDirty(dirtyZone);
  m_bannerView.reload();
}

void BoxView::reload() {
  markRectAsDirty(bounds());
  computeLabels(Axis::Horizontal);
  m_bannerView.reload();
}

int BoxView::selectedQuantile() {
  return m_selectedQuantile;
}

bool BoxView::selectQuantile(int selectedQuantile) {
  if (selectedQuantile < 0 || selectedQuantile > 4) {
    return false;
  }
  if (m_selectedQuantile != selectedQuantile) {
    reloadSelection();
    m_selectedQuantile = selectedQuantile;
    reloadSelection();
  }
  return true;
}

void BoxView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawAxes(ctx, rect, Axis::Horizontal);
  drawLabels(ctx, rect, Axis::Horizontal, false);
  float calculations[5] = {m_data->minValue(), m_data->firstQuartile(), m_data->median(), m_data->thirdQuartile(), m_data->maxValue()};
  float lowBounds[5] = {0.4f, 0.2f, 0.2f, 0.2f, 0.4f};
  float upBounds[5] = {0.6f, 0.8f, 0.8f, 0.8f, 0.6f};
  for (int k = 0; k < 5; k++) {
    drawSegment(ctx, rect, Axis::Vertical, calculations[k], lowBounds[k], upBounds[k], KDColorBlack);
  }
  if (m_mainViewSelected) {
    drawSegment(ctx, rect, Axis::Vertical, calculations[m_selectedQuantile], lowBounds[m_selectedQuantile], upBounds[m_selectedQuantile], KDColorRed);
  }
  drawSegment(ctx, rect, Axis::Horizontal, 0.5f, m_data->minValue(), m_data->firstQuartile(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.5f, m_data->thirdQuartile(), m_data->maxValue(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.2f, m_data->firstQuartile(), m_data->thirdQuartile(), KDColorBlack);
  drawSegment(ctx, rect, Axis::Horizontal, 0.8f, m_data->firstQuartile(), m_data->thirdQuartile(), KDColorBlack);
}

char * BoxView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    return nullptr;
  }
  return (char *)m_labels[index];
}

View * BoxView::bannerView() {
  return &m_bannerView;
}

}
