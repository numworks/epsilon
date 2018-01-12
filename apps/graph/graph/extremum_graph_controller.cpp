#include "extremum_graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

ExtremumGraphController::ExtremumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, I18n::Message defaultMessage) :
  ViewController(parentResponder),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange),
  m_cursor(cursor),
  m_function(nullptr),
  m_defaultBannerView(KDText::FontSize::Small, defaultMessage, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_isActive(false)
{
}

View * ExtremumGraphController::view() {
  return m_graphView;
}

void ExtremumGraphController::viewWillAppear() {
  assert(m_function != nullptr);
  CartesianFunction::Point p = computeExtremumBetweenBounds(m_graphRange->xMin(), m_graphRange->xMax());
  if (std::isnan(p.abscissa)) {
    m_isActive = false;
    m_graphView->setCursorView(nullptr);
    m_graphView->setBannerView(&m_defaultBannerView);
  } else {
    m_isActive = true;
    m_cursor->moveTo(p.abscissa, p.value);
    m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, SimpleInteractiveCurveViewController::k_cursorRightMarginRatio, k_cursorBottomMarginRatio, SimpleInteractiveCurveViewController::k_cursorLeftMarginRatio);
    reloadBannerView();
  }
  m_graphView->setOkView(nullptr);
  m_graphView->reload();
}

bool ExtremumGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::EXE || event == Ion::Events::OK) {
    StackViewController * stack = static_cast<StackViewController *>(parentResponder());
    stack->pop();
    return true;
  }
  if (m_isActive && (event == Ion::Events::Right || event == Ion::Events::Left)) {
    int direction = event == Ion::Events::Right ? 1 : -1;
    if (moveCursor(direction)) {
      reloadBannerView();
      m_graphView->reload();
      return true;
    }
  }
  return false;
}

void ExtremumGraphController::setFunction(CartesianFunction * function) {
  m_graphView->selectFunction(function);
  m_function = function;
}

void ExtremumGraphController::reloadBannerView() {
  m_bannerView->setNumberOfSubviews(2);
  reloadBannerViewForCursorOnFunction(m_cursor, m_function, 'x');
}

bool ExtremumGraphController::moveCursor(int direction) {
  double x = m_cursor->x();
  float step = m_graphRange->xGridUnit()/SimpleInteractiveCurveViewController::k_numberOfCursorStepsInGradUnit;
  float start = direction > 0 ? x+step : m_graphRange->xMin();
  float end = direction > 0 ? m_graphRange->xMax() : x-step;
  CartesianFunction::Point newExtremum = computeExtremumBetweenBounds(start, end);
  if (std::isnan(newExtremum.abscissa)) {
    return false;
  }
  m_cursor->moveTo(newExtremum.abscissa, newExtremum.value);
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, SimpleInteractiveCurveViewController::k_cursorRightMarginRatio, k_cursorBottomMarginRatio, SimpleInteractiveCurveViewController::k_cursorLeftMarginRatio);
  return true;
}

MinimumGraphController::MinimumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
  ExtremumGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMinimumFound)
{
}

const char * MinimumGraphController::title() {
  return I18n::translate(I18n::Message::Minimum);
}

CartesianFunction::Point MinimumGraphController::computeExtremumBetweenBounds(float start, float end) {
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  return m_function->mininimumBetweenBounds(start, end, myApp->localContext());
}

}
