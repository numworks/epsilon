#include "tangent_graph_controller.h"
#include "../app.h"
#include <apps/apps_container_helper.h>
#include "../../shared/poincare_helpers.h"
#include <poincare/preferences.h>
#include <poincare/print.h>

using namespace Shared;
using namespace Poincare;
using namespace Escher;

namespace Graph {

TangentGraphController::TangentGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange)
{
}

const char * TangentGraphController::title() {
  return I18n::translate(I18n::Message::Tangent);
}

void TangentGraphController::viewWillAppear() {
  Shared::SimpleInteractiveCurveViewController::viewWillAppear();
  m_graphView->setTangentDisplay(true);
  m_graphView->setFocus(true);
  m_bannerView->setDisplayParameters(false, true, true);
  reloadBannerView();
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
  m_graphView->reload();
}

void TangentGraphController::didBecomeFirstResponder() {
  if (curveView()->hasFocus()) {
    m_bannerView->abscissaValue()->setParentResponder(this);
    m_bannerView->abscissaValue()->setDelegates(textFieldDelegateApp(), this);
    Container::activeApp()->setFirstResponder(m_bannerView->abscissaValue());
  }
}

bool TangentGraphController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  Shared::TextFieldDelegateApp * myApp = textFieldDelegateApp();
  double floatBody = textFieldDelegateApp()->parseInputtedFloatValue<double>(text);
  if (myApp->hasUndefinedValue(floatBody)) {
    return false;
  }
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(m_record);
  assert(function->properties().isCartesian());
  double y = function->evaluate2DAtParameter(floatBody, myApp->localContext()).x2();
  m_cursor->moveTo(floatBody, floatBody, y);
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
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
  FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(m_cursor, m_record, Shared::FunctionApp::app()->functionStore(), AppsContainerHelper::sharedAppsContainerGlobalContext());

  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  int precision = numberOfSignificantDigits();

  double coefficientA = GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, m_record);
  Poincare::Print::CustomPrintf(buffer, bufferSize, "a=%*.*ed", coefficientA, Poincare::Preferences::sharedPreferences->displayMode(), precision);
  m_bannerView->aView()->setText(buffer);

  double coefficientB = -coefficientA * m_cursor->x() + m_cursor->y();
  Poincare::Print::CustomPrintf(buffer, bufferSize, "b=%*.*ed", coefficientB, Poincare::Preferences::sharedPreferences->displayMode(), precision);
  m_bannerView->bView()->setText(buffer);
  m_bannerView->reload();
}

bool TangentGraphController::moveCursorHorizontally(OMG::NewHorizontalDirection direction, int scrollSpeed) {
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, m_record, curveView()->pixelWidth());
}

bool TangentGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

}
