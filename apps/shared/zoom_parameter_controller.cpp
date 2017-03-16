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
  m_legends{PointerTextView(KDText::FontSize::Small, I18n::Message::TopCapital, 0.0f, 0.5f, KDColorBlack, Palette::GreyBright),
    PointerTextView(KDText::FontSize::Small, I18n::Message::LeftCapital, 0.0f, 0.5f, KDColorBlack, Palette::GreyBright),
    PointerTextView(KDText::FontSize::Small, I18n::Message::ZoomCapital, 0.5f, 0.5f, KDColorBlack, Palette::GreyBright),
    PointerTextView(KDText::FontSize::Small, I18n::Message::BottomCapital, 1.0f, 0.5f, KDColorBlack, Palette::GreyBright),
    PointerTextView(KDText::FontSize::Small, I18n::Message::RightCapital, 1.0f, 0.5f, KDColorBlack, Palette::GreyBright)},
  m_legendPictograms{KeyView(KeyView::Type::Up), KeyView(KeyView::Type::Left),
    KeyView(KeyView::Type::Plus), KeyView(KeyView::Type::Minus),
    KeyView(KeyView::Type::Down), KeyView(KeyView::Type::Right)}
{
}

void ZoomParameterController::ContentView::LegendView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(KDRect(0, bounds().height() - k_legendHeight, bounds().width(), k_legendHeight), Palette::GreyBright);
}

int ZoomParameterController::ContentView::LegendView::numberOfSubviews() const {
  return 2*k_numberOfLegends-1;
}

View * ZoomParameterController::ContentView::LegendView::subviewAtIndex(int index) {
  assert(index >= 0 && index < 2*k_numberOfLegends);
  if (index < k_numberOfLegends-1) {
    return &m_legends[index];
  }
  return &m_legendPictograms[index-k_numberOfLegends+1];
}

void ZoomParameterController::ContentView::LegendView::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  for (int row = 0; row < 2; row++) {
    m_legends[row].setFrame(KDRect(k_tokenWidth, row*height/2, width/3 - k_tokenWidth, height/2));
    m_legends[3+row].setFrame(KDRect(2*width/3, row*height/2, width/3 - k_tokenWidth, height/2));
  }
  m_legends[2].setFrame(KDRect(width/3, 0, width/3, height));
  for (int row = 0; row < 2; row++) {
    m_legendPictograms[row].setFrame(KDRect(0, row*height/2, k_tokenWidth, height/2));
    m_legendPictograms[4+row].setFrame(KDRect(width-k_tokenWidth, row*height/2, k_tokenWidth, height/2));
  }
  m_legendPictograms[2].setFrame(KDRect(width/3, 0, k_tokenWidth, height));
  m_legendPictograms[3].setFrame(KDRect(2*width/3-k_tokenWidth, 0, k_tokenWidth, height));
}

}
