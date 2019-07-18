#include "tangent_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

TangentGraphController::TangentGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange),
  m_record()
{
}

const char * TangentGraphController::title() {
  return I18n::translate(I18n::Message::Tangent);
}

void TangentGraphController::viewWillAppear() {
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
  m_graphView->drawTangent(true);
  m_graphView->setOkView(nullptr);
  m_graphView->selectMainView(true);
  m_bannerView->setNumberOfSubviews(BannerView::k_numberOfSubviews);
  reloadBannerView();
  m_graphView->reload();
}

void TangentGraphController::didBecomeFirstResponder() {
  if (curveView()->isMainViewSelected()) {
    m_bannerView->abscissaValue()->setParentResponder(this);
    m_bannerView->abscissaValue()->setDelegates(app(), this);
    app()->setFirstResponder(m_bannerView->abscissaValue());
  }
}

bool TangentGraphController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  double floatBody;
  if (app()->hasUndefinedValue(text, floatBody)) {
    return false;
  }
  ExpiringPointer<CartesianFunction> function = app()->functionStore()->modelForRecord(m_record);
  double y = function->evaluateAtAbscissa(floatBody, app()->localContext());
  m_cursor->moveTo(floatBody, y);
  interactiveCurveViewRange()->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
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
  FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(m_cursor, m_record, Shared::FunctionApp::app()->functionStore(), CartesianFunction::Symbol());
  GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, m_record);
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * legend = "a=";
  int legendLength = strlcpy(buffer, legend, bufferSize);
  ExpiringPointer<CartesianFunction> function = app()->functionStore()->modelForRecord(m_record);
  double y = function->approximateDerivative(m_cursor->x(), app()->localContext());
  PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView->aView()->setText(buffer);

  legend = "b=";
  legendLength = strlcpy(buffer, legend, bufferSize);
  y = -y*m_cursor->x()+function->evaluateAtAbscissa(m_cursor->x(), app()->localContext());
  PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView->bView()->setText(buffer);
  m_bannerView->reload();
}

bool TangentGraphController::moveCursorHorizontally(int direction) {
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, m_record);
}

bool TangentGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

}
