#include "calculation_graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

CalculationGraphController::CalculationGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, I18n::Message defaultMessage) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange),
  m_record(),
  m_defaultBannerView(BannerView::Font(), defaultMessage, 0.5f, 0.5f, BannerView::TextColor(), BannerView::BackgroundColor()),
  m_isActive(false)
{
}

void CalculationGraphController::viewWillAppear() {
  assert(!m_record.isNull());
  Coordinate2D<double> pointOfInterest = computeNewPointOfInteresetFromAbscissa(m_graphRange->xMin(), 1);
  if (std::isnan(pointOfInterest.x1())) {
    m_isActive = false;
    m_graphView->setCursorView(nullptr);
    m_graphView->setBannerView(&m_defaultBannerView);
  } else {
    m_isActive = true;
    assert(App::app()->functionStore()->modelForRecord(m_record)->plotType() == Shared::CartesianFunction::PlotType::Cartesian);
    m_cursor->moveTo(pointOfInterest.x1(), pointOfInterest.x1(), pointOfInterest.x2());
    m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
    m_bannerView->setNumberOfSubviews(Shared::XYBannerView::k_numberOfSubviews);
    reloadBannerView();
  }
  m_graphView->setOkView(nullptr);
  m_graphView->reload();
}

void CalculationGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

void CalculationGraphController::reloadBannerView() {
  reloadBannerViewForCursorOnFunction(m_cursor, m_record, functionStore());
}

Coordinate2D<double> CalculationGraphController::computeNewPointOfInteresetFromAbscissa(double start, int direction) {
  double step = m_graphRange->xGridUnit()/10.0;
  step = direction < 0 ? -step : step;
  double max = direction > 0 ? m_graphRange->xMax() : m_graphRange->xMin();
  return computeNewPointOfInterest(start, step, max, textFieldDelegateApp()->localContext());
}

CartesianFunctionStore * CalculationGraphController::functionStore() const {
  return App::app()->functionStore();
}

bool CalculationGraphController::handleLeftRightEvent(Ion::Events::Event event) {
  if (!m_isActive) {
    return false;
  }
  return SimpleInteractiveCurveViewController::handleLeftRightEvent(event);
}

bool CalculationGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

bool CalculationGraphController::moveCursorHorizontally(int direction) {
  Coordinate2D<double> newPointOfInterest = computeNewPointOfInteresetFromAbscissa(m_cursor->x(), direction);
  if (std::isnan(newPointOfInterest.x1())) {
    return false;
  }
  assert(App::app()->functionStore()->modelForRecord(m_record)->plotType() == Shared::CartesianFunction::PlotType::Cartesian);
  m_cursor->moveTo(newPointOfInterest.x1(), newPointOfInterest.x1(), newPointOfInterest.x2());
  return true;
}

}
