#include "function_zoom_and_pan_curve_view_controller.h"

#include <assert.h>

#include <cmath>

using namespace Escher;

namespace Shared {

FunctionZoomAndPanCurveViewController::FunctionZoomAndPanCurveViewController(
    Responder* parentResponder, InteractiveCurveViewRange* interactiveRange,
    AbstractPlotView* curveView)
    : ZoomAndPanCurveViewController(parentResponder),
      m_contentView(curveView),
      m_interactiveRange(interactiveRange) {}

const char* FunctionZoomAndPanCurveViewController::title() const {
  return I18n::translate(I18n::Message::Navigate);
}

void FunctionZoomAndPanCurveViewController::viewWillAppear() {
  setLegendVisible(true);
  ZoomAndPanCurveViewController::viewWillAppear();
}

void FunctionZoomAndPanCurveViewController::didBecomeFirstResponder() {
  m_contentView.layoutSubviews();
}

bool FunctionZoomAndPanCurveViewController::handleEvent(
    Ion::Events::Event event) {
  if (!event.isKeyPress() || event == Ion::Events::OnOff) {
    return false;
  }
  if (event == Ion::Events::Back || event == Ion::Events::Home ||
      event == Ion::Events::OK || event == Ion::Events::EXE) {
    setLegendVisible(false);
    return ZoomAndPanCurveViewController::handleEvent(event);
  }

  bool didHandleEvent = ZoomAndPanCurveViewController::handleEvent(event);
  bool didChangeLegend = setLegendVisible(!didHandleEvent);
  return (didHandleEvent || didChangeLegend);
}

float FunctionZoomAndPanCurveViewController::offscreenYAxis() const {
  /* We need to change the curve range to keep the same visual aspect of the
   * view. */
  float yMin = m_interactiveRange->yMin(), yMax = m_interactiveRange->yMax();
  /* We want the new graph to have the exact same pixel height as the old
   * one, to avoid seeing some grid lines move when entering navigation. */
  float oldPixelHeight = (yMax - yMin) / (k_standardViewHeight - 1);
  float newYMax = yMin + oldPixelHeight * (m_contentView.bounds().height() - 1);
  float dY = newYMax - yMax;
  /* As we are adding space and the Y range that should not be taken into
   * account for computing the grid, we count it as negative offscreen. */
  return -dY;
}

/* Moving of a whole number of pixels is cache-friendly and greatly enhances
 * performances. */
float FunctionZoomAndPanCurveViewController::xMove() {
  return std::round(ZoomAndPanCurveViewController::xMove() /
                    m_contentView.curveView()->pixelWidth()) *
         m_contentView.curveView()->pixelWidth();
}

float FunctionZoomAndPanCurveViewController::yMove() {
  return std::round(ZoomAndPanCurveViewController::yMove() /
                    m_contentView.curveView()->pixelHeight()) *
         m_contentView.curveView()->pixelHeight();
}

bool FunctionZoomAndPanCurveViewController::setLegendVisible(
    bool legendWillAppear) {
  if (legendWillAppear == m_contentView.displayLegend()) {
    return false;
  }
  m_contentView.setDisplayLegend(legendWillAppear);
  m_contentView.layoutSubviews();
  return true;
}

/* Content View */

FunctionZoomAndPanCurveViewController::ContentView::ContentView(
    AbstractPlotView* curveView)
    : m_curveView(curveView), m_displayLegend(false) {}

int FunctionZoomAndPanCurveViewController::ContentView::numberOfSubviews()
    const {
  return 2;
}

View* FunctionZoomAndPanCurveViewController::ContentView::subviewAtIndex(
    int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_curveView;
  }
  return &m_legendView;
}

void FunctionZoomAndPanCurveViewController::ContentView::layoutSubviews(
    bool force) {
  setChildFrame(m_curveView, bounds(), force);
  setChildFrame(&m_legendView,
                m_displayLegend ? KDRect(0, bounds().height() - k_legendHeight,
                                         bounds().width(), k_legendHeight)
                                : KDRectZero,
                force);
}

AbstractPlotView*
FunctionZoomAndPanCurveViewController::ContentView::curveView() {
  return m_curveView;
}

/* Legend View */

FunctionZoomAndPanCurveViewController::ContentView::LegendView::LegendView()
    : m_legendPictograms{KeyView::Type::Up,   KeyView::Type::Down,
                         KeyView::Type::Left, KeyView::Type::Right,
                         KeyView::Type::Plus, KeyView::Type::Minus} {
  I18n::Message messages[k_numberOfLegends] = {
      I18n::Message::Move, I18n::Message::ToZoom, I18n::Message::Or};
  for (int i = 0; i < k_numberOfLegends; i++) {
    m_legends[i].setFont(ContentView::k_legendFont);
    m_legends[i].setMessage(messages[i]);
    m_legends[i].setBackgroundColor(k_backgroundColor);
    m_legends[i].setAlignment(KDGlyph::k_alignCenter, KDGlyph::k_alignCenter);
  }
}

void FunctionZoomAndPanCurveViewController::ContentView::LegendView::drawRect(
    KDContext* ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(),
                       k_legendHeight),
                k_backgroundColor);
}

int FunctionZoomAndPanCurveViewController::ContentView::LegendView::
    numberOfSubviews() const {
  return k_numberOfLegends + k_numberOfTokens;
}

View* FunctionZoomAndPanCurveViewController::ContentView::LegendView::
    subviewAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfTokens + k_numberOfLegends);
  if (index < k_numberOfLegends) {
    return &m_legends[index];
  }
  return &m_legendPictograms[index - k_numberOfLegends];
}

void FunctionZoomAndPanCurveViewController::ContentView::LegendView::
    layoutSubviews(bool force) {
  /* We want to imitate a banner with two elements, the first one being
   * "Pan: ^v<>" and the other being "Zoom: + or -". */
  KDCoordinate legendWidth[k_numberOfLegends];
  KDCoordinate totalLegendWidth = 0;
  for (int i = 0; i < k_numberOfLegends; i++) {
    legendWidth[i] = m_legends[i].minimalSizeForOptimalDisplay().width();
    totalLegendWidth += legendWidth[i];
  }
  KDCoordinate halfSpacing =
      (bounds().width() - totalLegendWidth - k_tokenWidth * k_numberOfTokens) /
      4;
  KDCoordinate height = bounds().height();

  KDCoordinate x = halfSpacing;
  setChildFrame(&m_legends[0], KDRect(x, 0, legendWidth[0], height), force);
  x += legendWidth[0];
  for (int i = 0; i < k_numberOfTokens - 2; i++) {
    setChildFrame(&m_legendPictograms[i], KDRect(x, 0, k_tokenWidth, height),
                  force);
    x += k_tokenWidth;
  }
  x += 2 * halfSpacing;
  for (int i = 1; i < k_numberOfLegends; i++) {
    setChildFrame(&m_legends[i], KDRect(x, 0, legendWidth[i], height), force);
    x += legendWidth[i];
    setChildFrame(&m_legendPictograms[k_numberOfTokens - 3 + i],
                  KDRect(x, 0, k_tokenWidth, height), force);
    x += k_tokenWidth;
  }
}

}  // namespace Shared
