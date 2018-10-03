#include "storage_tangent_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

StorageTangentGraphController::StorageTangentGraphController(Responder * parentResponder, StorageGraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  SimpleInteractiveCurveViewController(parentResponder, curveViewRange, graphView, cursor),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange),
  m_function(nullptr)
{
}

const char * StorageTangentGraphController::title() {
  return I18n::translate(I18n::Message::Tangent);
}

void StorageTangentGraphController::viewWillAppear() {
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  m_graphView->drawTangent(true);
  m_graphView->setOkView(nullptr);
  m_graphView->selectMainView(true);
  reloadBannerView();
  m_graphView->reload();
}

void StorageTangentGraphController::setFunction(StorageCartesianFunction * function) {
  m_graphView->selectFunction(function);
  m_function = *function;
}

void StorageTangentGraphController::reloadBannerView() {
  m_bannerView->setNumberOfSubviews(6);
  if (m_function.isEmpty()) {
    return;
  }
  StorageFunctionBannerDelegate<StorageCartesianFunction>::reloadBannerViewForCursorOnFunction(m_cursor, &m_function, 'x');
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  //TODO GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, m_function, myApp);
  char buffer[FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * legend = "a=";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  double y = m_function.approximateDerivative(m_cursor->x(), myApp->localContext());
  PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView->setLegendAtIndex(buffer, 4);

  legend = "b=";
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  y = -y*m_cursor->x()+m_function.evaluateAtAbscissa(m_cursor->x(), myApp->localContext());
  PoincareHelpers::ConvertFloatToText<double>(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView->setLegendAtIndex(buffer, 5);
}

bool StorageTangentGraphController::moveCursorHorizontally(int direction) {
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  return true; //TODO privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, m_function, myApp, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

bool StorageTangentGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

}
