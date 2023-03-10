#include "calculation_graph_controller.h"

#include <apps/apps_container_helper.h>
#include <escher/clipboard.h>

#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

CalculationGraphController::CalculationGraphController(
    Responder* parentResponder, GraphView* graphView, BannerView* bannerView,
    Shared::InteractiveCurveViewRange* curveViewRange, CurveViewCursor* cursor,
    I18n::Message defaultMessage)
    : SimpleInteractiveCurveViewController(parentResponder, cursor),
      m_graphView(graphView),
      m_bannerView(bannerView),
      m_graphRange(curveViewRange),
      m_defaultBannerView(defaultMessage,
                          {{.glyphColor = BannerView::TextColor(),
                            .backgroundColor = BannerView::BackgroundColor(),
                            .font = BannerView::k_font},
                           .horizontalAlignment = KDGlyph::k_alignCenter}),
      m_isActive(false) {}

bool CalculationGraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Idle) {
    // Compute the points of interest when the user is not active
    m_graphView->resumePointsOfInterestDrawing();
    return true;
  }
  if (event == Ion::Events::Copy || event == Ion::Events::Cut) {
    Escher::Clipboard::SharedClipboard()->store(
        m_bannerView->abscissaValue()->text());
    return true;
  }
  if (event == Ion::Events::Sto || event == Ion::Events::Var) {
    App::app()->storeValue(m_bannerView->abscissaValue()->text());
    return true;
  }
  return SimpleInteractiveCurveViewController::handleEvent(event);
}

bool CalculationGraphController::handleEnter() {
  /* When leaving calculation, the displayed precision might get better than the
   * calculation one, highlighting precision errors. To prevent that, cursor is
   * moved to the value displayed on the banner. */
  double t = m_cursor->t();
  t = FunctionBannerDelegate::GetValueDisplayedOnBanner(
      t, App::app()->localContext(), numberOfSignificantDigits(),
      curveView()->pixelWidth());
  Coordinate2D<double> xy =
      App::app()
          ->functionStore()
          ->modelForRecord(m_record)
          ->evaluateXYAtParameter(t, App::app()->localContext());
  m_cursor->moveTo(t, xy.x(), xy.y());
  return Shared::SimpleInteractiveCurveViewController::handleEnter();
}

void CalculationGraphController::viewWillAppear() {
  Shared::SimpleInteractiveCurveViewController::viewWillAppear();
  curveView()->setFocus(true);
  assert(!m_record.isNull());
  Coordinate2D<double> pointOfInterest = computeNewPointOfInterestFromAbscissa(
      m_graphRange->xMin(), OMG::Direction::Right());
  if (std::isnan(pointOfInterest.x())) {
    m_isActive = false;
    m_graphView->setCursorView(nullptr);
    m_graphView->setBannerView(&m_defaultBannerView);
  } else {
    m_isActive = true;
    assert(App::app()
               ->functionStore()
               ->modelForRecord(m_record)
               ->properties()
               .isCartesian());
    m_cursor->moveTo(pointOfInterest.x(), pointOfInterest.x(),
                     pointOfInterest.y());
    m_graphView->cursorView()->setHighlighted(specialInterest() !=
                                              Solver<double>::Interest::None);
    m_bannerView->setDisplayParameters(false, false, false);
    reloadBannerView();
    m_graphRange->panToMakePointVisible(
        m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(),
        cursorRightMarginRatio(), cursorBottomMarginRatio(),
        cursorLeftMarginRatio(), curveView()->pixelWidth());
  }
  m_graphView->setInterest(specialInterest());
  m_graphView->reload();
}

void CalculationGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

void CalculationGraphController::reloadBannerView() {
  reloadBannerViewForCursorOnFunction(
      m_cursor, m_record, functionStore(),
      AppsContainerHelper::sharedAppsContainerGlobalContext());
}

Coordinate2D<double>
CalculationGraphController::computeNewPointOfInterestFromAbscissa(
    double start, OMG::HorizontalDirection direction) {
  double max =
      direction.isRight() ? m_graphRange->xMax() : m_graphRange->xMin();
  functionStore()->modelForRecord(m_record)->trimResolutionInterval(&start,
                                                                    &max);
  return computeNewPointOfInterest(start, max,
                                   textFieldDelegateApp()->localContext());
}

PointOfInterest CalculationGraphController::computeAtLeastOnePointOfInterest(
    double start, double max, Poincare::Context* context) {
  // Compute at least 1 point of interest before displaying the view
  PointsOfInterestCache* pointsOfInterest =
      App::app()->graphController()->pointsOfInterestForSelectedRecord();
  while (pointsOfInterest->numberOfPoints(specialInterest()) == 0 &&
         !pointsOfInterest->isFullyComputed()) {
    if (!pointsOfInterest->computeNextStep(false)) {
      /* Next step computation overflowed the pool. Returning an empty point of
       * interest will wrongfully display that nothing has been found in the
       * window. TODO : Fix this behavior. */
      break;
    }
  }
  return pointsOfInterest->firstPointInDirection(start, max, specialInterest());
}

ContinuousFunctionStore* CalculationGraphController::functionStore() const {
  return App::app()->functionStore();
}

bool CalculationGraphController::moveCursorHorizontally(
    OMG::HorizontalDirection direction, int scrollspeed) {
  if (!m_isActive) {
    return false;
  }
  Coordinate2D<double> newPointOfInterest =
      computeNewPointOfInterestFromAbscissa(m_cursor->x(), direction);
  if (std::isnan(newPointOfInterest.x())) {
    return false;
  }
  assert(App::app()
             ->functionStore()
             ->modelForRecord(m_record)
             ->properties()
             .isCartesian());
  m_cursor->moveTo(newPointOfInterest.x(), newPointOfInterest.x(),
                   newPointOfInterest.y());
  return true;
}

}  // namespace Graph
