#include "zoom_parameter_controller.h"
#include <assert.h>
#include <cmath>

namespace Shared {

ZoomParameterController::ZoomParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange, CurveView * curveView) :
  ViewController(parentResponder),
  m_contentView(curveView),
  m_interactiveRange(interactiveRange)
{
}

const char * ZoomParameterController::title() {
  return I18n::translate(I18n::Message::Zoom);
}

View * ZoomParameterController::view() {
  return &m_contentView;
}

bool ZoomParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus) {
    float meanX = (m_interactiveRange->xMin()+m_interactiveRange->xMax())/2.0f;
    float meanY = (m_interactiveRange->yMin()+m_interactiveRange->yMax())/2.0f;
    m_interactiveRange->zoom(2.0f/3.0f, meanX, meanY);
    m_contentView.curveView()->reload();
    return true;
  }
  if (event == Ion::Events::Minus) {
    float meanX = (m_interactiveRange->xMin()+m_interactiveRange->xMax())/2.0f;
    float meanY = (m_interactiveRange->yMin()+m_interactiveRange->yMax())/2.0f;
    m_interactiveRange->zoom(3.0f/2.0f, meanX, meanY);
    m_contentView.curveView()->reload();
    return true;
  }
  if (event == Ion::Events::Up) {
    m_interactiveRange->panWithVector(0.0f, m_interactiveRange->yGridUnit());
    m_contentView.curveView()->reload();
    return true;
  }
  if (event == Ion::Events::Down) {
    m_interactiveRange->panWithVector(0.0f, -m_interactiveRange->yGridUnit());
    m_contentView.curveView()->reload();
    return true;
  }
  if (event == Ion::Events::Left) {
    m_interactiveRange->panWithVector(-m_interactiveRange->xGridUnit(), 0.0f);
    m_contentView.curveView()->reload();
    return true;
  }
  if (event == Ion::Events::Right) {
    m_interactiveRange->panWithVector(m_interactiveRange->xGridUnit(), 0.0f);
    m_contentView.curveView()->reload();
    return true;
  }

  return false;
}

void ZoomParameterController::viewWillAppear() {
  m_contentView.curveView()->setOkView(nullptr);
}

void ZoomParameterController::didBecomeFirstResponder() {
  m_contentView.layoutSubviews();
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
  if (index == 0) {
    return m_curveView;
  }
  return &m_legendView;
}

void ZoomParameterController::ContentView::layoutSubviews() {
  m_curveView->setFrame(KDRect(0, 0, bounds().width(), bounds().height() - k_legendHeight));
  m_legendView.setFrame(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight));
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
    m_legends[i].setFontSize(KDText::FontSize::Small);
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

void ZoomParameterController::ContentView::LegendView::layoutSubviews() {
  KDCoordinate height = bounds().height();
  KDCoordinate xOrigin = 0;
  KDCoordinate legendWidth = m_legends[0].minimalSizeForOptimalDisplay().width();
  m_legends[0].setFrame(KDRect(xOrigin, 0, legendWidth, height));
  xOrigin += legendWidth;
  for (int i = 0; i < k_numberOfTokens - 2; i++) {
    m_legendPictograms[i].setFrame(KDRect(xOrigin, 0, k_tokenWidth, height));
    xOrigin += k_tokenWidth;
  }
  xOrigin = bounds().width()/2;
  for (int i = 1; i < k_numberOfLegends; i++) {
    KDCoordinate legendWidth = m_legends[i].minimalSizeForOptimalDisplay().width();
    m_legends[i].setFrame(KDRect(xOrigin, 0, legendWidth, height));
    xOrigin += legendWidth;
    m_legendPictograms[k_numberOfTokens - 3 + i].setFrame(KDRect(xOrigin, 0, k_tokenWidth, height));
    xOrigin += k_tokenWidth;
  }
}

}
