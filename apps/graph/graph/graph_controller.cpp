#include "graph_controller.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

GraphController::GraphController(Responder * parentResponder, CartesianFunctionStore * functionStore, HeaderViewController * header) :
  FunctionGraphController(parentResponder, header, &m_graphRange, &m_view),
  m_bannerView(BannerView()),
  m_view(GraphView(functionStore, &m_graphRange, &m_cursor, &m_bannerView, &m_cursorView)),
  m_graphRange(InteractiveCurveViewRange(&m_cursor, this)),
  m_curveParameterController(CurveParameterController(&m_graphRange, &m_bannerView, &m_cursor)),
  m_functionStore(functionStore)
{
}

const char * GraphController::emptyMessage() {
  if (m_functionStore->numberOfDefinedFunctions() == 0) {
    return "Aucune fonction";
  }
  return "Aucune fonction activee";
}

BannerView * GraphController::bannerView() {
  return &m_bannerView;
}

void GraphController::reloadBannerView() {
  FunctionGraphController::reloadBannerView();
  if (!m_bannerView.displayDerivative()) {
    return;
  }
  char buffer[k_maxNumberOfCharacters+Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  const char * legend = "00(x)=";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  if (m_functionStore->numberOfActiveFunctions() == 0) {
    return;
  }
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  buffer[0] = f->name()[0];
  buffer[1] = '\'';
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = f->approximateDerivative(m_cursor.x(), myApp->localContext());
  Complex::convertFloatToText(y, buffer + legendLength, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_bannerView.setLegendAtIndex(buffer, 2);
}

bool GraphController::moveCursorHorizontally(int direction) {
  float xCursorPosition = m_cursor.x();
  float x = direction > 0 ? xCursorPosition + m_graphRange.xGridUnit()/k_numberOfCursorStepsInGradUnit :
    xCursorPosition -  m_graphRange.xGridUnit()/k_numberOfCursorStepsInGradUnit;
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  float y = f->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor.moveTo(x, y);
  m_graphRange.panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

void GraphController::initCursorParameters() {
  float x = (interactiveCurveViewRange()->xMin()+interactiveCurveViewRange()->xMax())/2.0f;
  m_indexFunctionSelectedByCursor = 0;
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  int functionIndex = 0;
  float y = 0;
  do {
    CartesianFunction * firstFunction = functionStore()->activeFunctionAtIndex(functionIndex++);
    y = firstFunction->evaluateAtAbscissa(x, myApp->localContext());
  } while (isnan(y) && functionIndex < functionStore()->numberOfActiveFunctions());
  m_cursor.moveTo(x, y);
  interactiveCurveViewRange()->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

InteractiveCurveViewRange * GraphController::interactiveCurveViewRange() {
  return &m_graphRange;
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
