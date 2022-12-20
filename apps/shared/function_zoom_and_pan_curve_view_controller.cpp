#include "function_zoom_and_pan_curve_view_controller.h"
#include <assert.h>
#include <cmath>

using namespace Escher;

namespace Shared {

FunctionZoomAndPanCurveViewController::FunctionZoomAndPanCurveViewController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange, AbstractPlotView * curveView) :
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
  /* We need to change the curve range to keep the same visual aspect of the
   * view. */
  adaptRangeForHeaders(true);
  setLegendVisible(true);
  /* Force a reload in case some curves were interrupted. */
  m_contentView.curveView()->reload(true);
}

void FunctionZoomAndPanCurveViewController::didBecomeFirstResponder() {
  m_contentView.layoutSubviews();
}

bool FunctionZoomAndPanCurveViewController::handleEvent(Ion::Events::Event & event) {
  if (event == Ion::Events::Idle) {
    return false;
  }
  if (event == Ion::Events::Back || event == Ion::Events::Home || event == Ion::Events::OK || event == Ion::Events::EXE) {
    setLegendVisible(false);
    adaptRangeForHeaders(false);
    return ZoomAndPanCurveViewController::handleEvent(event);
  }

  bool didHandleEvent = ZoomAndPanCurveViewController::handleEvent(event);
  bool didChangeLegend = setLegendVisible(!didHandleEvent);
  /* If we intercept the OnOff event, the device will still be suspended by
   * preemption, but when turning back on, we would prevent the AppsContainer
   * from redrawing the whole frame, leaving the screen filled with noise, with
   * only the legend visible.
   * FIXME: We might want to simply not pass the OnOff event through
   * handleEvent ? */
  return (didHandleEvent || didChangeLegend) && (event != Ion::Events::OnOff);
}

void FunctionZoomAndPanCurveViewController::adaptRangeForHeaders(bool viewWillAppear) {
  assert(!m_contentView.displayLegend());
  float yMin = m_interactiveRange->yMin(), yMax = m_interactiveRange->yMax();
  if (viewWillAppear) {
    assert(m_interactiveRange->offscreenYAxis() == 0.f);
    /* We want the new graph to have the exact same pixel height as the old
     * one, to avoid seeing some grid lines move when entering navigation. */
    float oldPixelHeight = (yMax - yMin) / (k_standardViewHeight - 1);
    float newYMax = yMin + oldPixelHeight * (m_contentView.bounds().height() - 1);
    float dY = newYMax - yMax;
    m_interactiveRange->setOffscreenYAxis(-dY);
    /* As we are adding space and the Y range that should not be taken into
     * account for computing the grid, we count it as negative offscreen. */
  } else {
    m_interactiveRange->setOffscreenYAxis(0.f);
  }
}

bool FunctionZoomAndPanCurveViewController::setLegendVisible(bool legendWillAppear) {
  if (legendWillAppear == m_contentView.displayLegend()) {
    return false;
  }
  m_contentView.setDisplayLegend(legendWillAppear);
  m_contentView.layoutSubviews();
  return true;
}

/* Content View */

FunctionZoomAndPanCurveViewController::ContentView::ContentView(AbstractPlotView * curveView) :
  m_curveView(curveView),
  m_displayLegend(false)
{
}

int FunctionZoomAndPanCurveViewController::ContentView::numberOfSubviews() const {
  return 2;
}

View * FunctionZoomAndPanCurveViewController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_curveView;
  }
  return &m_legendView;
}

void FunctionZoomAndPanCurveViewController::ContentView::layoutSubviews(bool force) {
  m_curveView->setFrame(bounds(), force);
  m_legendView.setFrame(m_displayLegend ? KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight) : KDRectZero, force);
}

AbstractPlotView * FunctionZoomAndPanCurveViewController::ContentView::curveView() {
  return m_curveView;
}

/* Legend View */

FunctionZoomAndPanCurveViewController::ContentView::LegendView::LegendView()
{
  I18n::Message messages[k_numberOfLegends] = {I18n::Message::Move, I18n::Message::ToZoom, I18n::Message::Or};
  for (int i = 0; i < k_numberOfLegends; i++) {
    m_legends[i].setFont(ContentView::k_legendFont);
    m_legends[i].setMessage(messages[i]);
    m_legends[i].setBackgroundColor(BackgroundColor());
    m_legends[i].setAlignment(KDContext::k_alignCenter, KDContext::k_alignCenter);
  }
  KeyView::Type tokenTypes[k_numberOfTokens] = {KeyView::Type::Up, KeyView::Type::Down, KeyView::Type::Left, KeyView::Type::Right, KeyView::Type::Plus, KeyView::Type::Minus};
  for (int i = 0; i < k_numberOfTokens ; i++) {
    m_legendPictograms[i].setType(tokenTypes[i]);
  }
}

void FunctionZoomAndPanCurveViewController::ContentView::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), BackgroundColor());
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
  /* We want to imitate a banner with two elements, the first one being
   * "Pan: ^v<>" and the other being "Zoom: + or -". */
  KDCoordinate legendWidth[k_numberOfLegends];
  KDCoordinate totalLegendWidth = 0;
  for (int i = 0; i < k_numberOfLegends; i++) {
    legendWidth[i] = m_legends[i].minimalSizeForOptimalDisplay().width();
    totalLegendWidth += legendWidth[i];
  }
  KDCoordinate halfSpacing = (bounds().width() - totalLegendWidth - k_tokenWidth * k_numberOfTokens) / 4;
  KDCoordinate height = bounds().height();

  KDCoordinate x = halfSpacing;
  m_legends[0].setFrame(KDRect(x, 0, legendWidth[0], height), force);
  x += legendWidth[0];
  for (int i = 0; i < k_numberOfTokens - 2; i++) {
    m_legendPictograms[i].setFrame(KDRect(x, 0, k_tokenWidth, height), force);
    x += k_tokenWidth;
  }
  x += 2 * halfSpacing;
  for (int i = 1; i < k_numberOfLegends; i++) {
    m_legends[i].setFrame(KDRect(x, 0, legendWidth[i], height), force);
    x += legendWidth[i];
    m_legendPictograms[k_numberOfTokens - 3 + i].setFrame(KDRect(x, 0, k_tokenWidth, height), force);
    x += k_tokenWidth;
  }
}

}
