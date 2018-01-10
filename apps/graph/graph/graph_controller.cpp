#include "graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

GraphController::GraphController(Responder * parentResponder, CartesianFunctionStore * functionStore, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion, Expression::AngleUnit * angleUnitVersion, ButtonRowController * header) :
  FunctionGraphController(parentResponder, header, curveViewRange, &m_view, cursor, modelVersion, rangeVersion, angleUnitVersion),
  m_bannerView(),
  m_view(functionStore, curveViewRange, m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(curveViewRange),
  m_curveParameterController(curveViewRange, &m_bannerView, m_cursor, &m_view, this),
  m_functionStore(functionStore),
  m_displayDerivativeInBanner(false)
{
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (m_functionStore->numberOfDefinedFunctions() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

void GraphController::viewWillAppear() {
  m_view.drawTangent(false);
  FunctionGraphController::viewWillAppear();
}

bool GraphController::displayDerivativeInBanner() const {
  return m_displayDerivativeInBanner;
}

void GraphController::setDisplayDerivativeInBanner(bool displayDerivative) {
  m_displayDerivativeInBanner = displayDerivative;
}

BannerView * GraphController::bannerView() {
  return &m_bannerView;
}

void GraphController::reloadBannerView() {
  FunctionGraphController::reloadBannerView();
  m_bannerView.setNumberOfSubviews(2+m_displayDerivativeInBanner);
  if (m_functionStore->numberOfActiveFunctions() == 0 || !m_displayDerivativeInBanner) {
    return;
  }
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, f, myApp);
}

bool GraphController::moveCursorHorizontally(int direction) {
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, f, myApp, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

void GraphController::initCursorParameters() {
  double x = (interactiveCurveViewRange()->xMin()+interactiveCurveViewRange()->xMax())/2.0f;
  m_indexFunctionSelectedByCursor = 0;
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  int functionIndex = 0;
  double y = 0;
  do {
    CartesianFunction * firstFunction = functionStore()->activeFunctionAtIndex(functionIndex++);
    y = firstFunction->evaluateAtAbscissa(x, myApp->localContext());
  } while (std::isnan(y) && functionIndex < functionStore()->numberOfActiveFunctions());
  m_cursor->moveTo(x, y);
  interactiveCurveViewRange()->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

InteractiveCurveViewRange * GraphController::interactiveCurveViewRange() {
  return m_graphRange;
}

CartesianFunctionStore * GraphController::functionStore() const {
  return m_functionStore;
}

GraphView * GraphController::functionGraphView() {
  return &m_view;
}

CurveParameterController * GraphController::curveParameterController() {
  return &m_curveParameterController;
}

}
