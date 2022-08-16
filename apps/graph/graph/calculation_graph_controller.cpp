#include "calculation_graph_controller.h"
#include "../app.h"
#include <apps/apps_container_helper.h>
#include <escher/clipboard.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

CalculationGraphController::CalculationGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, I18n::Message defaultMessage) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange),
  m_defaultBannerView(BannerView::k_font, defaultMessage, KDContext::k_alignCenter, KDContext::k_alignCenter,
                      BannerView::TextColor(), BannerView::BackgroundColor()),
  m_isActive(false)
{
}

bool CalculationGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    Escher::Clipboard::sharedClipboard()->store(m_bannerView->abscissaValue()->text());
    return true;
  }
  return SimpleInteractiveCurveViewController::handleEvent(event);
}

bool CalculationGraphController::handleEnter() {
  /* When leaving calculation, the displayed precision might get better than the
   * calculation one, highlighting precision errors. To prevent that, cursor is
   * moved to the value displayed on the banner. */
  double t = m_cursor->t();
  t = FunctionBannerDelegate::getValueDisplayedOnBanner(t, App::app()->localContext(), numberOfSignificantDigits(), curveView()->pixelWidth());
  Coordinate2D<double> xy = App::app()->functionStore()->modelForRecord(m_record)->evaluateXYAtParameter(t, App::app()->localContext());
  m_cursor->moveTo(t, xy.x1(), xy.x2());
  return Shared::SimpleInteractiveCurveViewController::handleEnter();
}

void CalculationGraphController::viewWillAppear() {
  Shared::SimpleInteractiveCurveViewController::viewWillAppear();
  assert(!m_record.isNull());
  Coordinate2D<double> pointOfInterest = computeNewPointOfInterestFromAbscissa(m_graphRange->xMin(), 1);
  if (std::isnan(pointOfInterest.x1())) {
    m_isActive = false;
    m_graphView->setCursorView(nullptr);
    m_graphView->setBannerView(&m_defaultBannerView);
  } else {
    m_isActive = true;
    assert(App::app()->functionStore()->modelForRecord(m_record)->isAlongXorY());
    m_cursor->moveTo(pointOfInterest.x1(), pointOfInterest.x1(), pointOfInterest.x2());
    m_bannerView->setNumberOfSubviews(Shared::XYBannerView::k_numberOfSubviews);
    reloadBannerView();
    m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
  }
  m_graphView->reload();
}

void CalculationGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

void CalculationGraphController::reloadBannerView() {
  reloadBannerViewForCursorOnFunction(m_cursor, m_record, functionStore(), AppsContainerHelper::sharedAppsContainerGlobalContext());
}

Coordinate2D<double> CalculationGraphController::computeNewPointOfInterestFromAbscissa(double start, int direction) {
  double max = direction > 0 ? m_graphRange->xMax() : m_graphRange->xMin();
  return computeNewPointOfInterest(start, max, textFieldDelegateApp()->localContext(), Solver::k_relativePrecision, Solver::k_minimalStep, Solver::DefaultMaximalStep(start, max));
}

ContinuousFunctionStore * CalculationGraphController::functionStore() const {
  return App::app()->functionStore();
}

bool CalculationGraphController::moveCursorHorizontally(int direction, int scrollspeed) {
  if (!m_isActive) {
    return false;
  }
  Coordinate2D<double> newPointOfInterest = computeNewPointOfInterestFromAbscissa(m_cursor->x(), direction);
  if (std::isnan(newPointOfInterest.x1())) {
    return false;
  }
  assert(App::app()->functionStore()->modelForRecord(m_record)->isAlongXorY());
  m_cursor->moveTo(newPointOfInterest.x1(), newPointOfInterest.x1(), newPointOfInterest.x2());
  return true;
}

}
