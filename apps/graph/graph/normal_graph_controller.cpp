#include "normal_graph_controller.h"

#include <apps/apps_container_helper.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/preferences.h>
#include <poincare/print.h>

#include "../app.h"

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

NormalGraphController::NormalGraphController(
    Responder *parentResponder, GraphView *graphView, BannerView *bannerView,
    Shared::InteractiveCurveViewRange *curveViewRange, CurveViewCursor *cursor)
    : SimpleInteractiveCurveViewController(parentResponder, cursor),
      m_graphView(graphView),
      m_bannerView(bannerView),
      m_graphRange(curveViewRange) {}

const char *NormalGraphController::title() {
  return I18n::translate(I18n::Message::NormalLine);
}

void NormalGraphController::viewWillAppear() {
  Shared::SimpleInteractiveCurveViewController::viewWillAppear();
  m_graphView->setNormalDisplay(true);
  m_graphView->setFocus(true);
  m_bannerView->setDisplayParameters(false, true, false, true);
  reloadBannerView();
  panToMakeCursorVisible();
  Shared::SimpleInteractiveCurveViewController::viewWillAppear();
}

void NormalGraphController::didBecomeFirstResponder() {
  if (curveView()->hasFocus()) {
    m_bannerView->abscissaValue()->setParentResponder(this);
    m_bannerView->abscissaValue()->setDelegate(this);
    App::app()->setFirstResponder(m_bannerView->abscissaValue());
  }
}

bool NormalGraphController::textFieldDidFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  double floatBody = ParseInputFloatValue<double>(textField->draftText());
  if (HasUndefinedValue(floatBody)) {
    return false;
  }
  ExpiringPointer<ContinuousFunction> function =
      App::app()->functionStore()->modelForRecord(m_record);
  assert(function->properties().isCartesian());
  double y =
      function->evaluate2DAtParameter(floatBody, App::app()->localContext())
          .y();
  m_cursor->moveTo(floatBody, floatBody, y);
  panToMakeCursorVisible();
  reloadBannerView();
  curveView()->reload();
  return true;
}

void NormalGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

void NormalGraphController::reloadBannerView() {
  if (m_record.isNull()) {
    return;
  }
  FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(
      m_cursor, m_record, Shared::FunctionApp::app()->functionStore(),
      AppsContainerHelper::sharedAppsContainerGlobalContext());

  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  int precision = numberOfSignificantDigits();

  double coefficientA =
      -1 / GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(
          m_cursor, m_record);
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "a=%*.*ed", coefficientA,
      Poincare::Preferences::sharedPreferences->displayMode(), precision);
  m_bannerView->aView()->setText(buffer);

  double coefficientB = -coefficientA * m_cursor->x() + m_cursor->y();
  Poincare::Print::CustomPrintf(
      buffer, bufferSize, "b=%*.*ed", coefficientB,
      Poincare::Preferences::sharedPreferences->displayMode(), precision);
  m_bannerView->bView()->setText(buffer);
  m_bannerView->reload();
}

bool NormalGraphController::moveCursorHorizontally(
    OMG::HorizontalDirection direction, int scrollSpeed) {
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange,
                                       k_numberOfCursorStepsInGradUnit,
                                       m_record, curveView()->pixelWidth());
}

bool NormalGraphController::handleEnter() {
  StackViewController *stack =
      static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

}  // namespace Graph
