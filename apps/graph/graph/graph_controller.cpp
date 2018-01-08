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
  m_type(Type::Default)
{
  m_graphRange->setDelegate(this);
}

const char * GraphController::title() {
  switch(m_type) {
    case Type::Tangent:
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
  if (m_type == Type::Default || event == Ion::Events::Left || event == Ion::Events::Right || event == Ion::Events::Plus || event == Ion::Events::Minus) {
    return FunctionGraphController::handleEvent(event);
  }
  // TODO: handle for type != Type::Default
  if (m_type != Type::Default && event == Ion::Events::Back) {
    stackController()->handleEvent(event);
    setType(Type::Default);
    setParentResponder(static_cast<App *>(app())->graphControllerParent());
    return true;
  }
  return false;
}

void GraphController::setType(Type type) {
  m_type = type;
}

BannerView * GraphController::bannerView() {
  return &m_bannerView;
}

void GraphController::reloadBannerView() {
  // TODO: do something else if m_type == Type::tangent
  FunctionGraphController::reloadBannerView();
  if (!m_bannerView.displayDerivative()) {
    return;
  }
  char buffer[k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
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
  double y = f->approximateDerivative(m_cursor->x(), myApp->localContext());
  Complex<double>::convertFloatToText(y, buffer + legendLength, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  m_bannerView.setLegendAtIndex(buffer, 2);
}

bool GraphController::moveCursorHorizontally(int direction) {
  double xCursorPosition = m_cursor->x();
  double x = direction > 0 ? xCursorPosition + m_graphRange->xGridUnit()/k_numberOfCursorStepsInGradUnit :
    xCursorPosition -  m_graphRange->xGridUnit()/k_numberOfCursorStepsInGradUnit;
  CartesianFunction * f = m_functionStore->activeFunctionAtIndex(m_indexFunctionSelectedByCursor);
  TextFieldDelegateApp * myApp = (TextFieldDelegateApp *)app();
  double y = f->evaluateAtAbscissa(x, myApp->localContext());
  m_cursor->moveTo(x, y);
  m_graphRange->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
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
  if (m_type != Type::Default) {
    return (StackViewController *)(parentResponder());
  }
  return FunctionGraphController::stackController();
}

}
