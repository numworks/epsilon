#include "graph_controller.h"
#include "../app.h"

using namespace Shared;

namespace Graph {

static inline float maxFloat(float x, float y) { return x > y ? x : y; }

GraphController::GraphController(Responder * parentResponder, ::InputEventHandlerDelegate * inputEventHandlerDelegate, CartesianFunctionStore * functionStore, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * rangeVersion, Poincare::Preferences::AngleUnit * angleUnitVersion, ButtonRowController * header) :
  FunctionGraphController(parentResponder, inputEventHandlerDelegate, header, curveViewRange, &m_view, cursor, indexFunctionSelectedByCursor, modelVersion, rangeVersion, angleUnitVersion),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(functionStore, curveViewRange, m_cursor, &m_bannerView, &m_cursorView),
  m_graphRange(curveViewRange),
  m_curveParameterController(inputEventHandlerDelegate, curveViewRange, &m_bannerView, m_cursor, &m_view, this),
  m_displayDerivativeInBanner(false)
{
  m_graphRange->setDelegate(this);
}

I18n::Message GraphController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

void GraphController::viewWillAppear() {
  m_view.drawTangent(false);
  m_view.setCursorView(&m_cursorView);
  m_bannerView.setNumberOfSubviews(Shared::XYBannerView::k_numberOfSubviews + m_displayDerivativeInBanner);
  FunctionGraphController::viewWillAppear();
  selectFunctionWithCursor(indexFunctionSelectedByCursor()); // update the color of the cursor
}

bool GraphController::displayDerivativeInBanner() const {
  return m_displayDerivativeInBanner;
}

void GraphController::setDisplayDerivativeInBanner(bool displayDerivative) {
  m_displayDerivativeInBanner = displayDerivative;
}

float GraphController::interestingXHalfRange() const {
  float characteristicRange = 0.0f;
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    float fRange = f->expressionReduced(context).characteristicXRange(*context, Poincare::Preferences::sharedPreferences()->angleUnit());
    if (!std::isnan(fRange)) {
      characteristicRange = maxFloat(fRange, characteristicRange);
    }
  }
  return (characteristicRange > 0.0f ? 1.6f*characteristicRange : InteractiveCurveViewRangeDelegate::interestingXHalfRange());
}

int GraphController::estimatedBannerNumberOfLines() const {
  return 1 + m_displayDerivativeInBanner;
}

void GraphController::selectFunctionWithCursor(int functionIndex) {
  FunctionGraphController::selectFunctionWithCursor(functionIndex);
  ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(functionIndex));
  m_cursorView.setColor(f->color());
}

BannerView * GraphController::bannerView() {
  return &m_bannerView;
}

void GraphController::reloadBannerView() {
  FunctionGraphController::reloadBannerView();
  if (functionStore()->numberOfActiveFunctions() == 0 || !m_displayDerivativeInBanner) {
    return;
  }
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, record);
}

bool GraphController::moveCursorHorizontally(int direction) {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, record);
}

InteractiveCurveViewRange * GraphController::interactiveCurveViewRange() {
  return m_graphRange;
}

GraphView * GraphController::functionGraphView() {
  return &m_view;
}

CurveParameterController * GraphController::curveParameterController() {
  return &m_curveParameterController;
}

}
