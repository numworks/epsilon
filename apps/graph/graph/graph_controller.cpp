#include "graph_controller.h"
#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

static inline float minFloat(float x, float y) { return x < y ? x : y; }
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
  FunctionGraphController::viewWillAppear();
  selectFunctionWithCursor(indexFunctionSelectedByCursor());
}

void GraphController::interestingFunctionRange(ExpiringPointer<CartesianFunction> f, float tMin, float tMax, float step, float * xm, float * xM, float * ym, float * yM) const {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  const int balancedBound = std::floor((tMax-tMin)/2/step);
  for (int j = -balancedBound; j <= balancedBound ; j++) {
    float t = (tMin+tMax)/2 + step * j;
    Coordinate2D<float> xy = f->evaluateXYAtParameter(t, context);
    float x = xy.x1();
    float y = xy.x2();
    if (!std::isnan(x) && !std::isinf(x) && !std::isnan(y) && !std::isinf(y)) {
      *xm = minFloat(*xm, x);
      *xM = maxFloat(*xM, x);
      *ym = minFloat(*ym, y);
      *yM = maxFloat(*yM, y);
    }
  }
}

void GraphController::interestingRanges(float * xm, float * xM, float * ym, float * yM) const {
  float resultxMin = FLT_MAX;
  float resultxMax = -FLT_MAX;
  float resultyMin = FLT_MAX;
  float resultyMax = -FLT_MAX;
  assert(functionStore()->numberOfActiveFunctions() > 0);
  if (displaysNonCartesianFunctions()) {
    const int functionsCount = functionStore()->numberOfActiveFunctions();
    for (int i = 0; i < functionsCount; i++) {
      ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
      if (f->plotType() == CartesianFunction::PlotType::Cartesian) {
        continue;
      }
      /* Scan x-range from the middle to the extrema in order to get balanced
       * y-range for even functions (y = 1/x). */
      double tMin = f->tMin();
      double tMax = f->tMax();
      assert(!std::isnan(tMin));
      assert(!std::isnan(tMax));
      assert(!std::isnan(f->rangeStep()));
      interestingFunctionRange(f, tMin, tMax, f->rangeStep(), &resultxMin, &resultxMax, &resultyMin, &resultyMax);
    }
    if (resultxMin > resultxMax) {
      resultxMin = - Range1D::k_default;
      resultxMax = Range1D::k_default;
    }
  } else {
    resultxMin = const_cast<GraphController *>(this)->interactiveCurveViewRange()->xMin();
    resultxMax = const_cast<GraphController *>(this)->interactiveCurveViewRange()->xMax();
  }
  /* In practice, a step smaller than a pixel's width is needed for sampling
   * the values of a function. Otherwise some relevant extremal values may be
   * missed. */
  const float step = const_cast<GraphController *>(this)->curveView()->pixelWidth() / 2;
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    if (f->plotType() != CartesianFunction::PlotType::Cartesian) {
      continue;
    }
    /* Scan x-range from the middle to the extrema in order to get balanced
     * y-range for even functions (y = 1/x). */
    assert(!std::isnan(f->tMin()));
    assert(!std::isnan(f->tMax()));
    double tMin = maxFloat(f->tMin(), resultxMin);
    double tMax = minFloat(f->tMax(), resultxMax);
    interestingFunctionRange(f, tMin, tMax, step, &resultxMin, &resultxMax, &resultyMin, &resultyMax);
  }
  if (resultyMin > resultyMax) {
    resultyMin = - Range1D::k_default;
    resultyMax = Range1D::k_default;
  }

  *xm = resultxMin;
  *xM = resultxMax;
  *ym = resultyMin;
  *yM = resultyMax;
}

float GraphController::interestingXHalfRange() const {
  float characteristicRange = 0.0f;
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  CartesianFunctionStore * store = functionStore();
  for (int i = 0; i < store->numberOfActiveFunctions(); i++) {
    ExpiringPointer<CartesianFunction> f = store->modelForRecord(store->activeRecordAtIndex(i));
    float fRange = f->expressionReduced(context).characteristicXRange(context, Poincare::Preferences::sharedPreferences()->angleUnit());
    if (!std::isnan(fRange)) {
      characteristicRange = maxFloat(fRange, characteristicRange);
    }
  }
  return (characteristicRange > 0.0f ? 1.6f*characteristicRange : InteractiveCurveViewRangeDelegate::interestingXHalfRange());
}

void GraphController::selectFunctionWithCursor(int functionIndex) {
  FunctionGraphController::selectFunctionWithCursor(functionIndex);
  ExpiringPointer<CartesianFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(functionIndex));
  m_cursorView.setColor(f->color());
}

void GraphController::reloadBannerView() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  bool displayDerivative = m_displayDerivativeInBanner &&
    functionStore()->modelForRecord(record)->plotType() == CartesianFunction::PlotType::Cartesian;
  m_bannerView.setNumberOfSubviews(Shared::XYBannerView::k_numberOfSubviews + displayDerivative);
  FunctionGraphController::reloadBannerView();
  if (!displayDerivative) {
    return;
  }
  reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, record);
}

bool GraphController::moveCursorHorizontally(int direction) {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, record);
}

int GraphController::nextCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context) const {
  int nbOfActiveFunctions = functionStore()-> numberOfActiveFunctions();
  if (functionStore()->numberOfActiveFunctionsOfType(CartesianFunction::PlotType::Cartesian) == nbOfActiveFunctions) {
    return FunctionGraphController::nextCurveIndexVertically(goingUp, currentSelectedCurve, context);
  }
  int nextActiveFunctionIndex = currentSelectedCurve + (goingUp ? -1 : 1);
  return nextActiveFunctionIndex >= nbOfActiveFunctions ? -1 : nextActiveFunctionIndex;
}

double GraphController::defaultCursorT(Ion::Storage::Record record) {
  ExpiringPointer<CartesianFunction> function = functionStore()->modelForRecord(record);
  if (function->plotType() == CartesianFunction::PlotType::Cartesian) {
    return FunctionGraphController::defaultCursorT(record);
  }
  return function->tMin();
}

bool GraphController::displaysNonCartesianFunctions() const {
  CartesianFunctionStore * store = functionStore();
  return store->numberOfActiveFunctionsOfType(CartesianFunction::PlotType::Polar) > 0
    || store->numberOfActiveFunctionsOfType(CartesianFunction::PlotType::Parametric) > 0;
}

bool GraphController::shouldSetDefaultOnModelChange() const {
  return displaysNonCartesianFunctions();
}

}
