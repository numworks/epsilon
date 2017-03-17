#include "zoom_parameter_controller.h"
#include <assert.h>
#include <math.h>

namespace Shared {

ZoomParameterController::ZoomParameterController(Responder * parentResponder, InteractiveCurveViewRange * interactiveRange, CurveView * curveView) :
  ViewController(parentResponder),
  m_contentView(ContentView(curveView)),
  m_interactiveRange(interactiveRange)
{
}

const char * ZoomParameterController::title() {
  return I18n::translate(I18n::Message::InteractiveZoom);
}

View * ZoomParameterController::view() {
  return &m_contentView;
}

bool ZoomParameterController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Plus) {
    m_interactiveRange->zoom(1.0f/3.0f);
    m_contentView.curveView()->reload();
    return true;
  }
  if (event == Ion::Events::Minus) {
    m_interactiveRange->zoom(3.0f/4.0f);
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
  m_curveView->setFrame(bounds());
  m_legendView.setFrame(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight));
}

CurveView * ZoomParameterController::ContentView::curveView() {
  return m_curveView;
}

/* Legend View */

ZoomParameterController::ContentView::LegendView::LegendView() :
  m_legends{MessageTextView(KDText::FontSize::Small, I18n::Message::Move, 1.0f, 0.5f, KDColorBlack, Palette::GreyBright),
    MessageTextView(KDText::FontSize::Small, I18n::Message::ToZoom, 1.0f, 0.5f, KDColorBlack, Palette::GreyBright),
    MessageTextView(KDText::FontSize::Small, I18n::Message::Or, 0.5f, 0.5f, KDColorBlack, Palette::GreyBright)},
  m_legendPictograms{KeyView(KeyView::Type::Up), KeyView(KeyView::Type::Down),
    KeyView(KeyView::Type::Left), KeyView(KeyView::Type::Right),
    KeyView(KeyView::Type::Plus), KeyView(KeyView::Type::Minus)}
{
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
