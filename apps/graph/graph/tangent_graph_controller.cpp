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
  m_graphView->drawTangent(true);
  m_graphView->selectMainView(true);
  m_bannerView->setNumberOfSubviews(BannerView::k_numberOfSubviews);
  reloadBannerView();
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), cursorRightMarginRatio(), cursorBottomMarginRatio(), cursorLeftMarginRatio(), curveView()->pixelWidth());
  m_graphView->reload();
}

void TangentGraphController::didBecomeFirstResponder() {
  if (curveView()->isMainViewSelected()) {
    m_bannerView->abscissaValue()->setParentResponder(this);
    m_bannerView->abscissaValue()->setDelegates(textFieldDelegateApp(), this);
    Container::activeApp()->setFirstResponder(m_bannerView->abscissaValue());
  }
}

bool TangentGraphController::textFieldDidFinishEditing(AbstractTextField * textField, const char * text, Ion::Events::Event event) {
  Shared::TextFieldDelegateApp * myApp = textFieldDelegateApp();
  double floatBody;
  if (myApp->hasUndefinedValue(text, &floatBody)) {
    return false;
  }
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(m_record);
  assert(function->isAlongXOrY());
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
  GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, m_record);
  constexpr size_t bufferSize = FunctionBannerDelegate::k_textBufferSize;
  char buffer[bufferSize];
  Poincare::Context * context = textFieldDelegateApp()->localContext();

  int precision = numberOfSignificantDigits();
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(m_record);
  assert(function->canDisplayDerivative());
  double y = function->approximateDerivative(m_cursor->x(), context);
  Poincare::Print::customPrintf(buffer, bufferSize, "a=%*.*ed", y, Poincare::Preferences::sharedPreferences()->displayMode(), precision);
  m_bannerView->aView()->setText(buffer);

  assert(function->isAlongXOrY());
  y = -y*m_cursor->x()+function->evaluate2DAtParameter(m_cursor->x(), context).x2();
  Poincare::Print::customPrintf(buffer, bufferSize, "b=%*.*ed", y, Poincare::Preferences::sharedPreferences()->displayMode(), precision);
  m_bannerView->bView()->setText(buffer);
  m_bannerView->reload();
}

bool TangentGraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, m_record, curveView()->pixelWidth());
}

bool TangentGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

}
