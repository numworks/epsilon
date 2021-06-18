#include "function_zoom_and_pan_curve_view_controller.h"
#include <assert.h>
#include <cmath>

namespace Shared {

FunctionZoomAndPanCurveViewController::FunctionZoomAndPanCurveViewController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange, CurveView * curveView) :
  ZoomAndPanCurveViewController(parentResponder),
  m_contentView(curveView),
  m_interactiveRange(interactiveRange),
  m_restoreZoomAuto(false)
{
}

const char * FunctionZoomAndPanCurveViewController::title() {
  return I18n::translate(I18n::Message::Navigate);
}

void FunctionZoomAndPanCurveViewController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_contentView.curveView()->setOkView(nullptr);
  /* We need to change the curve range to keep the same visual aspect of the
   * view. */
  adaptCurveRange(true);
}

void FunctionZoomAndPanCurveViewController::viewDidDisappear() {
  // Restore the curve range
  adaptCurveRange(false);
}

void FunctionZoomAndPanCurveViewController::didBecomeFirstResponder() {
  m_contentView.layoutSubviews();
}

bool FunctionZoomAndPanCurveViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Back) {
    /* If Auto is still on (because the navigation menu was brought up and
     * closed immediately), we need to deactivate it to prevent the range from
     * being recomputed in InteractiveCurveViewController::viewWillAppear().
     * We need to store it's state to reset it later in viewDidDisappear(), so
     * that open navigation without moving doesn't deactivate the Auto. */
    m_restoreZoomAuto = m_interactiveRange->zoomAuto();
    m_interactiveRange->setZoomAuto(false);
  }
  return ZoomAndPanCurveViewController::handleEvent(event);
}

void FunctionZoomAndPanCurveViewController::adaptCurveRange(bool viewWillAppear) {
  float currentYMin = m_interactiveRange->yMin();
  float currentRange = m_interactiveRange->yMax() - m_interactiveRange->yMin();
  float newYMin = 0;
  if (viewWillAppear) {
    float rangeOffscreen = ((float)ContentView::k_legendHeight)/((float)k_standardViewHeight)*currentRange;
    newYMin = currentYMin + rangeOffscreen;
    m_interactiveRange->setOffscreenYAxis(rangeOffscreen);
  } else {
    newYMin = m_interactiveRange->yMax() - currentRange*((float)k_standardViewHeight)/(((float)k_standardViewHeight)-((float)ContentView::k_legendHeight));
    m_interactiveRange->setOffscreenYAxis(0.f);
    m_interactiveRange->setZoomAuto(m_restoreZoomAuto);
    m_restoreZoomAuto = false;
  }
  m_interactiveRange->setYMin(newYMin);
  m_contentView.curveView()->reload();
}

/* Content View */

FunctionZoomAndPanCurveViewController::ContentView::ContentView(CurveView * curveView) :
  m_curveView(curveView)
{
}

int FunctionZoomAndPanCurveViewController::ContentView::numberOfSubviews() const {
  return 2;
}

View * FunctionZoomAndPanCurveViewController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  /* The order of subviews matters here: redrawing curve view can be long and
   * if it was redraw before the legend view, you could see noise when
   * switching the device on and off. */
  if (index == 0) {
    return &m_legendView;
  }
  return m_curveView;
}

void FunctionZoomAndPanCurveViewController::ContentView::layoutSubviews(bool force) {
  assert(bounds().height() == FunctionZoomAndPanCurveViewController::k_standardViewHeight);
  m_curveView->setFrame(KDRect(0, 0, bounds().width(), bounds().height() - k_legendHeight), force);
  m_legendView.setFrame(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), force);
}

CurveView * FunctionZoomAndPanCurveViewController::ContentView::curveView() {
  return m_curveView;
}

/* Legend View */

FunctionZoomAndPanCurveViewController::ContentView::LegendView::LegendView()
{
  I18n::Message messages[k_numberOfLegends] = {I18n::Message::Move, I18n::Message::ToZoom, I18n::Message::Or};
  float horizontalAlignments[k_numberOfLegends] = {1.0f, 1.0f, 0.5f};
  for (int i = 0; i < k_numberOfLegends; i++) {
    m_legends[i].setFont(KDFont::SmallFont);
    m_legends[i].setMessage(messages[i]);
    m_legends[i].setBackgroundColor(Palette::SubMenuBackground);
    m_legends[i].setAlignment(horizontalAlignments[i], 0.5f);
  }
  KeyView::Type tokenTypes[k_numberOfTokens] = {KeyView::Type::Up, KeyView::Type::Down, KeyView::Type::Left, KeyView::Type::Right, KeyView::Type::Plus, KeyView::Type::Minus};
  for (int i = 0; i < k_numberOfTokens ; i++) {
    m_legendPictograms[i].setType(tokenTypes[i]);
  }
}

void FunctionZoomAndPanCurveViewController::ContentView::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), Palette::SubMenuBackground);
}

int FunctionZoomAndPanCurveViewController::ContentView::LegendView::numberOfSubviews() const {
  return k_numberOfLegends+k_numberOfTokens;
}

View * FunctionZoomAndPanCurveViewController::ContentView::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfTokens+k_numberOfLegends);
  if (index < k_numberOfLegends) {
    return &m_legends[index];
  }
  return &m_legendPictograms[index-k_numberOfLegends];
}

void FunctionZoomAndPanCurveViewController::ContentView::LegendView::layoutSubviews(bool force) {
  KDCoordinate height = bounds().height();
  KDCoordinate xOrigin = 0;
  KDCoordinate legendWidth = m_legends[0].minimalSizeForOptimalDisplay().width();
  m_legends[0].setFrame(KDRect(xOrigin, 0, legendWidth, height), force);
  xOrigin += legendWidth;
  for (int i = 0; i < k_numberOfTokens - 2; i++) {
    m_legendPictograms[i].setFrame(KDRect(xOrigin, 0, k_tokenWidth, height), force);
    xOrigin += k_tokenWidth;
  }
  xOrigin = bounds().width()/2;
  for (int i = 1; i < k_numberOfLegends; i++) {
    KDCoordinate legendWidth = m_legends[i].minimalSizeForOptimalDisplay().width();
    m_legends[i].setFrame(KDRect(xOrigin, 0, legendWidth, height), force);
    xOrigin += legendWidth;
    m_legendPictograms[k_numberOfTokens - 3 + i].setFrame(KDRect(xOrigin, 0, k_tokenWidth, height), force);
    xOrigin += k_tokenWidth;
  }
}

}
