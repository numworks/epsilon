#include "tangent_graph_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

TangentGraphController::TangentGraphController(
    Responder* parentResponder, GraphView* graphView, BannerView* bannerView,
    InteractiveCurveViewRange* curveViewRange, CurveViewCursor* cursor)
    : SimpleInteractiveCurveViewController(parentResponder, cursor),
      m_graphView(graphView),
      m_bannerView(bannerView),
      m_graphRange(curveViewRange),
      m_drawTangent(false) {}

const char* TangentGraphController::title() const {
  I18n::Message message = m_drawTangent ? I18n::Message::Tangent
                                        : I18n::Message::CartesianSlopeFormula;
  return I18n::translate(message);
}

void TangentGraphController::viewWillAppear() {
  SimpleInteractiveCurveViewController::viewWillAppear();
  m_graphView->setTangentDisplay(m_drawTangent);
  m_graphView->setFocus(true);
  bool isCartesian = function()->properties().isCartesian();
  m_bannerView->setDisplayParameters({.showOrdinate = true,
                                      .showFirstDerivative = isCartesian,
                                      .showSlope = !isCartesian,
                                      .showTangent = m_drawTangent});
  reloadBannerView();
  panToMakeCursorVisible();
  SimpleInteractiveCurveViewController::viewWillAppear();
}

void TangentGraphController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    if (curveView()->hasFocus()) {
      m_bannerView->abscissaValue()->setParentResponder(this);
      m_bannerView->abscissaValue()->setDelegate(this);
      App::app()->setFirstResponder(m_bannerView->abscissaValue());
    }
  } else {
    SimpleInteractiveCurveViewController::handleResponderChainEvent(event);
  }
}

bool TangentGraphController::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  /* TODO: factorise with
   * InteractiveCurveViewController::textFieldDidFinishEditing */
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  ExpiringPointer<ContinuousFunction> f = function();
  Coordinate2D<double> xy =
      f->evaluateXYAtParameter(floatBody, App::app()->localContext());
  m_cursor->moveTo(floatBody, xy.x(), xy.y());
  panToMakeCursorVisible();
  reloadBannerView();
  curveView()->reload();
  return true;
}

void TangentGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

void TangentGraphController::reloadBannerView() {
  if (m_record.isNull()) {
    return;
  }
  FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(
      m_cursor->t(), m_cursor->x(), m_cursor->y(), m_record,
      FunctionApp::app()->functionStore(),
      AppsContainerHelper::sharedAppsContainerGlobalContext());

  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  int precision = numberOfSignificantDigits();

  double coefficientA;
  if (m_bannerView->showFirstDerivative()) {
    PointOrRealScalar<double> derivative =
        reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, m_record, 1);
    assert(derivative.isRealScalar());
    coefficientA = derivative.toRealScalar();
  } else {
    assert(m_bannerView->showSlope());
    coefficientA =
        reloadSlopeInBannerViewForCursorOnFunction(m_cursor, m_record);
  }

  Print::CustomPrintf(buffer, bufferSize, "a=%*.*ed", coefficientA,
                      Preferences::SharedPreferences()->displayMode(),
                      precision);
  m_bannerView->aView()->setText(buffer);

  double coefficientB = -coefficientA * m_cursor->x() + m_cursor->y();
  Print::CustomPrintf(buffer, bufferSize, "b=%*.*ed", coefficientB,
                      Preferences::SharedPreferences()->displayMode(),
                      precision);
  m_bannerView->bView()->setText(buffer);
  m_bannerView->reload();
}

bool TangentGraphController::moveCursorHorizontally(
    OMG::HorizontalDirection direction, int scrollSpeed) {
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange,
                                       k_numberOfCursorStepsInGradUnit,
                                       m_record, curveView()->pixelWidth());
}

bool TangentGraphController::handleEnter() {
  StackViewController* stack =
      static_cast<StackViewController*>(parentResponder());
  stack->pop();
  return true;
}

ExpiringPointer<ContinuousFunction> TangentGraphController::function() const {
  return App::app()->functionStore()->modelForRecord(m_record);
}

}  // namespace Graph
