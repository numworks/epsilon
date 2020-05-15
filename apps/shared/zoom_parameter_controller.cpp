#include "zoom_parameter_controller.h"
#include <assert.h>
#include <cmath>

namespace Shared {

ZoomParameterController::ZoomParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange, CurveView * curveView) :
  ZoomAndPanCurveViewController(parentResponder),
  m_contentView(curveView),
  m_interactiveRange(interactiveRange)
{
}

const char * ZoomParameterController::title() {
  return I18n::translate(I18n::Message::Zoom);
}

void ZoomParameterController::viewWillAppear() {
  ViewController::viewWillAppear();
  m_contentView.curveView()->setOkView(nullptr);
  /* We need to change the curve range to keep the same visual aspect of the
   * view. */
  adaptCurveRange(true);
}

void ZoomParameterController::viewDidDisappear() {
  // Restore the curve range
  adaptCurveRange(false);
}

void ZoomParameterController::didBecomeFirstResponder() {
  m_contentView.layoutSubviews();
}

void ZoomParameterController::adaptCurveRange(bool viewWillAppear) {
  float currentYMin = m_interactiveRange->yMin();
  float currentRange = m_interactiveRange->yMax() - m_interactiveRange->yMin();
  float newYMin = 0;
  if (viewWillAppear) {
    newYMin = currentYMin + ((float)ContentView::k_legendHeight)/((float)k_standardViewHeight)*currentRange;
  } else {
    newYMin = m_interactiveRange->yMax() - currentRange*((float)k_standardViewHeight)/(((float)k_standardViewHeight)-((float)ContentView::k_legendHeight));
  }
  m_interactiveRange->setYMin(newYMin);
  m_contentView.curveView()->reload();
}

/* Content View */

ZoomParameterController::ContentView::ContentView(CurveView * curveView) :
  m_curveView(curveView)
{
}

int ZoomParameterController::ContentView::numberOfSubviews() const {
  return 2;
}

View * ZoomParameterController::ContentView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2);
  /* The order of subview is important here :
   * If we redraw the curveView before the legendView, that can have some display issue, when exiting sleep mode, which
     can be visible, if the redraw of curveView is long (with complicated curve), so we prefer to have legendView
    at first subview.
  */
  if (index == 0) {
    return &m_legendView;
  }
  return m_curveView;
}

void ZoomParameterController::ContentView::layoutSubviews(bool force) {
  assert(bounds().height() == ZoomParameterController::k_standardViewHeight);
  m_curveView->setFrame(KDRect(0, 0, bounds().width(), bounds().height() - k_legendHeight), force);
  m_legendView.setFrame(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), force);
}

CurveView * ZoomParameterController::ContentView::curveView() {
  return m_curveView;
}

/* Legend View */

ZoomParameterController::ContentView::LegendView::LegendView()
{
  I18n::Message messages[k_numberOfLegends] = {I18n::Message::Move, I18n::Message::ToZoom, I18n::Message::Or};
  float horizontalAlignments[k_numberOfLegends] = {1.0f, 1.0f, 0.5f};
  for (int i = 0; i < k_numberOfLegends; i++) {
    m_legends[i].setFont(KDFont::SmallFont);
    m_legends[i].setMessage(messages[i]);
    m_legends[i].setBackgroundColor(Palette::GreyBright);
    m_legends[i].setAlignment(horizontalAlignments[i], 0.5f);
  }
  KeyView::Type tokenTypes[k_numberOfTokens] = {KeyView::Type::Up, KeyView::Type::Down, KeyView::Type::Left, KeyView::Type::Right, KeyView::Type::Plus, KeyView::Type::Minus};
  for (int i = 0; i < k_numberOfTokens ; i++) {
    m_legendPictograms[i].setType(tokenTypes[i]);
  }
}

void ZoomParameterController::ContentView::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), Palette::GreyBright);
}

int ZoomParameterController::ContentView::LegendView::numberOfSubviews() const {
  return k_numberOfLegends+k_numberOfTokens;
}

View * ZoomParameterController::ContentView::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < k_numberOfTokens+k_numberOfLegends);
  if (index < k_numberOfLegends) {
    return &m_legends[index];
  }
  return &m_legendPictograms[index-k_numberOfLegends];
}

void ZoomParameterController::ContentView::LegendView::layoutSubviews(bool force) {
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
