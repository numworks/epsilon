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
  m_curveParameterController(curveViewRange, &m_bannerView, m_cursor, this),
  m_functionStore(functionStore),
  m_displayDerivativeInBanner(false)
{
  m_graphRange->setDelegate(this);
}

const char * GraphController::title() {
  switch(type()) {
    case GraphView::Type::Tangent:
      return I18n::translate(I18n::Message::Tangent);
    default:
      return I18n::translate(I18n::Message::GraphTab);
  }
}

I18n::Message GraphController::emptyMessage() {
  if (m_functionStore->numberOfDefinedFunctions() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

bool GraphController::handleEvent(Ion::Events::Event event) {
  if (type() == GraphView::Type::Default || event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Plus || event == Ion::Events::Minus) {
    return FunctionGraphController::handleEvent(event);
  }
  if (type() != GraphView::Type::Default && (event == Ion::Events::Back || event == Ion::Events::OK)) {
    StackViewController * stack = stackController();
    setType(GraphView::Type::Default);
    App * a = static_cast<App *>(app());
    stack->handleEvent(Ion::Events::Back);
    setParentResponder(a->graphControllerParent());
    return true;
  }
  return false;
}

void GraphController::setType(GraphView::Type t) {
  m_view.setType(t);
  if (type() != GraphView::Type::Default) {
    m_view.selectFunction(m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor));
  } else {
    m_view.selectFunction(nullptr);
  }
}

bool GraphController::displayDerivativeInBanner() const {
  return m_displayDerivativeInBanner;
}

void GraphController::setDisplayDerivativeInBanner(bool displayDerivative) {
  m_displayDerivativeInBanner = displayDerivative;
}

GraphView::Type GraphController::type() const {
  return m_view.type();
}

BannerView * GraphController::bannerView() {
  return &m_bannerView;
}

void GraphController::reloadBannerView() {
  FunctionGraphController::reloadBannerView();
  m_bannerView.setNumberOfSubviews(2+m_displayDerivativeInBanner);
  if (type() == GraphView::Type::Tangent) {
    m_bannerView.setNumberOfSubviews(6);
  }
  if (m_functionStore->numberOfActiveFunctions() == 0) {
    return;
  }
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, f, myApp);


  char buffer[FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  if (type() == GraphView::Type::Tangent) {
    const char * legend = "a=";
    int legendLength = strlen(legend);
    strlcpy(buffer, legend, legendLength+1);
    double y = f->approximateDerivative(m_cursor->x(), myApp->localContext());
    Complex<double>::convertFloatToText(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
    m_bannerView.setLegendAtIndex(buffer, 4);

    legend = "b=";
    legendLength = strlen(legend);
    strlcpy(buffer, legend, legendLength+1);
    y = -y*m_cursor->x()+f->evaluateAtAbscissa(m_cursor->x(), myApp->localContext());
    Complex<double>::convertFloatToText(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
    m_bannerView.setLegendAtIndex(buffer, 5);
  }
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

StackViewController * GraphController::stackController() const{
  if (type() != GraphView::Type::Default) {
    return (StackViewController *)(parentResponder());
  }
  return FunctionGraphController::stackController();
}

}
