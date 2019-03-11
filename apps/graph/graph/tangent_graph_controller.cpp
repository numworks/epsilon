#include "tangent_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

TangentGraphController::TangentGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  SimpleInteractiveCurveViewController(parentResponder, curveViewRange, graphView, cursor),
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
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  m_graphView->drawTangent(true);
  m_graphView->setOkView(nullptr);
  m_graphView->selectMainView(true);
  reloadBannerView();
  m_graphView->reload();
}

void TangentGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

void TangentGraphController::reloadBannerView() {
  m_bannerView->setNumberOfSubviews(6);
  if (m_record.isNull()) {
    return;
  }
  App * myApp = static_cast<App *>(app());
  FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(m_cursor, m_record, myApp->functionStore(), CartesianFunction::Symbol());
  GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, m_record, myApp);
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * legend = "a=";
  int legendLength = strlcpy(buffer, legend, bufferSize);
  ExpiringPointer<CartesianFunction> function = myApp->functionStore()->modelForRecord(m_record);
  double y = function->approximateDerivative(m_cursor->x(), myApp->localContext());
  PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView->setLegendAtIndex(buffer, 4);

  legend = "b=";
  legendLength = strlcpy(buffer, legend, bufferSize);
  y = -y*m_cursor->x()+function->evaluateAtAbscissa(m_cursor->x(), myApp->localContext());
  PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView->setLegendAtIndex(buffer, 5);
}

bool TangentGraphController::moveCursorHorizontally(int direction) {
  App * myApp = static_cast<App *>(app());
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, m_record, myApp, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

bool TangentGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

}
