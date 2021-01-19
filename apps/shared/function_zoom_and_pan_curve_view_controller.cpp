#include "function_zoom_and_pan_curve_view_controller.h"
#include <assert.h>
#include <cmath>

using namespace Escher;

namespace Shared {

FunctionZoomAndPanCurveViewController::FunctionZoomAndPanCurveViewController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange, CurveView * curveView) :
  ZoomAndPanCurveViewController(parentResponder),
  m_contentView(curveView),
  m_interactiveRange(interactiveRange)
{
}

const char * FunctionZoomAndPanCurveViewController::title() {
  return I18n::translate(I18n::Message::Navigate);
}

void FunctionZoomAndPanCurveViewController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_contentView.curveView()->setOkView(nullptr);
  m_contentView.setDisplayLegend(true);
  /* We need to change the curve range to keep the same visual aspect of the
   * view. */
  adaptCurveRange(true);
}

void FunctionZoomAndPanCurveViewController::didBecomeFirstResponder() {
  m_contentView.layoutSubviews();
}

bool FunctionZoomAndPanCurveViewController::handleEvent(Ion::Events::Event event) {
  bool isQuitEvent = event == Ion::Events::Back || event == Ion::Events::Home;
  bool zoomAutoWasOn = m_interactiveRange->zoomAuto();
  /* Deactivate the automatic zoom to avoid recomputing the range if the user
   * immediately quits the menu. */
  m_interactiveRange->setZoomAuto(false);
  bool eventHasEffect = ZoomAndPanCurveViewController::handleEvent(event) || isQuitEvent;
  /* Hide the legend if the event does something, as the user likely does not
   * need it, but display it if the event does nothing to remind the user of
   * the UI.
   * This also hides the legend if the menu is going to be closed, therefore
   * calling adaptCurveRange(false) to reset the range to its correct values. */
  bool legendVisibilityChanged = setLegendVisible(!eventHasEffect);
  if (zoomAutoWasOn && (isQuitEvent || !eventHasEffect || event == Ion::Events::OK || event == Ion::Events::EXE)) {
    /* Reset the automatic zoom if the menu is closing or if nothing happened.
     * This way, entering garbage input does not deactivate the zoom. */
    m_interactiveRange->setZoomAuto(true);
  }
  return !isQuitEvent && (legendVisibilityChanged || eventHasEffect);
}

void FunctionZoomAndPanCurveViewController::adaptCurveRange(bool legendWillAppear) {
  float currentYMin = m_interactiveRange->yMin();
  float currentRange = m_interactiveRange->yMax() - m_interactiveRange->yMin();
  float newYMin = 0;
  if (legendWillAppear) {
    float rangeOffscreen = ((float)ContentView::k_legendHeight)/((float)k_standardViewHeight)*currentRange;
    newYMin = currentYMin + rangeOffscreen;
    m_interactiveRange->setOffscreenYAxis(rangeOffscreen);
  } else {
    newYMin = m_interactiveRange->yMax() - currentRange*((float)k_standardViewHeight)/(((float)k_standardViewHeight)-((float)ContentView::k_legendHeight));
    m_interactiveRange->setOffscreenYAxis(0.f);
  }
  m_interactiveRange->setYMin(newYMin);
  m_contentView.curveView()->reload();
}

bool FunctionZoomAndPanCurveViewController::setLegendVisible(bool visible) {
  if (visible != m_contentView.displayLegend()) {
    m_contentView.setDisplayLegend(visible);
    adaptCurveRange(visible);
    m_contentView.layoutSubviews();
    return true;
  }
  return false;
}

/* Content View */

FunctionZoomAndPanCurveViewController::ContentView::ContentView(CurveView * curveView) :
  m_curveView(curveView),
  m_displayLegend(true)
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
  KDCoordinate curveHeight = m_displayLegend ? bounds().height() - k_legendHeight : bounds().height();
  m_curveView->setFrame(KDRect(0, 0, bounds().width(), curveHeight), force);
  m_legendView.setFrame(m_displayLegend ? KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight) : KDRectZero, force);
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
    m_legends[i].setBackgroundColor(Palette::GrayBright);
    m_legends[i].setAlignment(horizontalAlignments[i], 0.5f);
  }
  KeyView::Type tokenTypes[k_numberOfTokens] = {KeyView::Type::Up, KeyView::Type::Down, KeyView::Type::Left, KeyView::Type::Right, KeyView::Type::Plus, KeyView::Type::Minus};
  for (int i = 0; i < k_numberOfTokens ; i++) {
    m_legendPictograms[i].setType(tokenTypes[i]);
  }
}

void FunctionZoomAndPanCurveViewController::ContentView::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), Palette::GrayBright);
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
