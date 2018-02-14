#include "tangent_graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

TangentGraphController::TangentGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  SimpleInteractiveCurveViewController(parentResponder, curveViewRange, graphView, cursor),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange),
  m_function(nullptr)
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

void TangentGraphController::setFunction(CartesianFunction * function) {
  m_graphView->selectFunction(function);
  m_function = function;
}

void TangentGraphController::reloadBannerView() {
  m_bannerView->setNumberOfSubviews(6);
  if (m_function == nullptr) {
    return;
  }
  FunctionBannerDelegate::reloadBannerViewForCursorOnFunction(m_cursor, m_function, 'x');
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, m_function, myApp);
  char buffer[FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * legend = "a=";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  double y = m_function->approximateDerivative(m_cursor->x(), myApp->localContext());
  PrintFloat::convertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView->setLegendAtIndex(buffer, 4);

  legend = "b=";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  y = -y*m_cursor->x()+m_function->evaluateAtAbscissa(m_cursor->x(), myApp->localContext());
  PrintFloat::convertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView->setLegendAtIndex(buffer, 5);
}

bool TangentGraphController::moveCursorHorizontally(int direction) {
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, m_function, myApp, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

bool TangentGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

}
