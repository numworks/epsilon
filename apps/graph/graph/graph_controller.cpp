#include "graph_controller.h"
#include "../app.h"
#include <algorithm>

using namespace Poincare;
using namespace Shared;

namespace Graph {

GraphController::GraphController(Responder * parentResponder, ::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, uint32_t * modelVersion, uint32_t * previousModelsVersions, uint32_t * rangeVersion, Poincare::Preferences::AngleUnit * angleUnitVersion, ButtonRowController * header) :
  FunctionGraphController(parentResponder, inputEventHandlerDelegate, header, curveViewRange, &m_view, cursor, indexFunctionSelectedByCursor, modelVersion, previousModelsVersions, rangeVersion, angleUnitVersion),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(curveViewRange, m_cursor, &m_bannerView, &m_cursorView),
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
#ifdef GRAPH_CURSOR_SPEEDUP
  m_cursorView.resetMemoization();
#endif
  m_view.setCursorView(&m_cursorView);
  FunctionGraphController::viewWillAppear();
  selectFunctionWithCursor(indexFunctionSelectedByCursor());
}

bool GraphController::defautRangeIsNormalized() const {
  return functionStore()->displaysNonCartesianFunctions();
}

void GraphController::interestingFunctionRange(ExpiringPointer<ContinuousFunction> f, float tMin, float tMax, float step, float * xm, float * xM, float * ym, float * yM) const {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  const int balancedBound = std::floor((tMax-tMin)/2/step);
  for (int j = -balancedBound; j <= balancedBound ; j++) {
    float t = (tMin+tMax)/2 + step * j;
    Coordinate2D<float> xy = f->evaluateXYAtParameter(t, context);
    float x = xy.x1();
    float y = xy.x2();
    if (!std::isnan(x) && !std::isinf(x) && !std::isnan(y) && !std::isinf(y)) {
      *xm = std::min(*xm, x);
      *xM = std::max(*xM, x);
      *ym = std::min(*ym, y);
      *yM = std::max(*yM, y);
    }
  }
}

void GraphController::interestingRanges(float * xm, float * xM, float * ym, float * yM) const {
  float resultxMin = FLT_MAX;
  float resultxMax = -FLT_MAX;
  float resultyMin = FLT_MAX;
  float resultyMax = -FLT_MAX;
  assert(functionStore()->numberOfActiveFunctions() > 0);
  int functionsCount = 0;
  if (functionStore()->displaysNonCartesianFunctions(&functionsCount)) {
    for (int i = 0; i < functionsCount; i++) {
      ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
      if (f->plotType() == ContinuousFunction::PlotType::Cartesian) {
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
  for (int i = 0; i < functionsCount; i++) {
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    if (f->plotType() != ContinuousFunction::PlotType::Cartesian) {
      continue;
    }
    /* Scan x-range from the middle to the extrema in order to get balanced
     * y-range for even functions (y = 1/x). */
    assert(!std::isnan(f->tMin()));
    assert(!std::isnan(f->tMax()));
    const double tMin = std::max(f->tMin(), resultxMin);
    const double tMax = std::min(f->tMax(), resultxMax);
    const double step = (tMax - tMin) / (2.0 * (m_view.bounds().width() - 1.0));
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
  ContinuousFunctionStore * store = functionStore();
  int nbActiveFunctions = store->numberOfActiveFunctions();
  double tMin = INFINITY;
  double tMax = -INFINITY;
  for (int i = 0; i < nbActiveFunctions; i++) {
    ExpiringPointer<ContinuousFunction> f = store->modelForRecord(store->activeRecordAtIndex(i));
    float fRange = f->expressionReduced(context).characteristicXRange(context, Poincare::Preferences::sharedPreferences()->angleUnit());
    if (!std::isnan(fRange) && !std::isinf(fRange)) {
      characteristicRange = std::max(fRange, characteristicRange);
    }
    // Compute the combined range of the functions
    assert(f->plotType() == ContinuousFunction::PlotType::Cartesian); // So that tMin tMax represents xMin xMax
    tMin = std::min<double>(tMin, f->tMin());
    tMax = std::max<double>(tMax, f->tMax());
  }
  constexpr float rangeMultiplicator = 1.6f;
  if (characteristicRange > 0.0f ) {
    return rangeMultiplicator * characteristicRange;
  }
  float defaultXHalfRange = InteractiveCurveViewRangeDelegate::interestingXHalfRange();
  assert(tMin <= tMax);
  if (tMin >= -defaultXHalfRange && tMax <= defaultXHalfRange) {
    /* If the combined Range of the functions is smaller than the default range,
     * use it. */
    float f = rangeMultiplicator * (float)std::max(std::fabs(tMin), std::fabs(tMax));
    return (std::isnan(f) || std::isinf(f)) ? defaultXHalfRange : f;
  }
  return defaultXHalfRange;
}

void GraphController::selectFunctionWithCursor(int functionIndex) {
  FunctionGraphController::selectFunctionWithCursor(functionIndex);
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(functionIndex));
  m_cursorView.setColor(f->color());
}

void GraphController::reloadBannerView() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  bool displayDerivative = m_displayDerivativeInBanner &&
    functionStore()->modelForRecord(record)->plotType() == ContinuousFunction::PlotType::Cartesian;
  m_bannerView.setNumberOfSubviews(Shared::XYBannerView::k_numberOfSubviews + displayDerivative);
  FunctionGraphController::reloadBannerView();
  if (!displayDerivative) {
    return;
  }
  reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, record);
}

bool GraphController::moveCursorHorizontally(int direction, bool fast) {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, record, fast);
}

int GraphController::nextCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context) const {
  int nbOfActiveFunctions = 0;
  if (!functionStore()->displaysNonCartesianFunctions(&nbOfActiveFunctions)) {
    return FunctionGraphController::nextCurveIndexVertically(goingUp, currentSelectedCurve, context);
  }
  int nextActiveFunctionIndex = currentSelectedCurve + (goingUp ? -1 : 1);
  return nextActiveFunctionIndex >= nbOfActiveFunctions ? -1 : nextActiveFunctionIndex;
}

double GraphController::defaultCursorT(Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
  if (function->plotType() == ContinuousFunction::PlotType::Cartesian) {
    return FunctionGraphController::defaultCursorT(record);
  }
  return function->tMin();
}

bool GraphController::shouldSetDefaultOnModelChange() const {
  return functionStore()->displaysNonCartesianFunctions();
}

void GraphController::jumpToLeftRightCurve(double t, int direction, int functionsCount, Ion::Storage::Record record) {
  if (functionsCount == 1) {
    return;
  }
  int nextCurveIndex = -1;
  double xDelta = DBL_MAX;
  double nextY = 0.0;
  double nextT = 0.0;
  for (int i = 0; i < functionsCount; i++) {
    Ion::Storage::Record currentRecord = functionStore()->activeRecordAtIndex(i);
    if (currentRecord == record) {
      continue;
    }
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(currentRecord);
    assert(f->plotType() == ContinuousFunction::PlotType::Cartesian);
    /* Select the closest horizontal curve, then the closest vertically, then
     * the lowest curve index. */
    double currentTMin = f->tMin();
    double currentTMax = f->tMax();
    assert(!std::isnan(currentTMin));
    assert(!std::isnan(currentTMax));
    if ((direction > 0 && currentTMax > t)
        ||(direction < 0 && currentTMin < t))
    {
      double currentXDelta = direction > 0 ?
        (t >= currentTMin ? 0.0 : currentTMin - t) :
        (t <= currentTMax ? 0.0 : t - currentTMax);
      assert(currentXDelta >= 0.0);
      if (currentXDelta <= xDelta) {
        double potentialNextTMin = f->tMin();
        double potentialNextTMax = f->tMax();
        double potentialNextT = std::max(potentialNextTMin, std::min(potentialNextTMax, t));
        Coordinate2D<double> xy = f->evaluateXYAtParameter(potentialNextT, App::app()->localContext());
        if (currentXDelta < xDelta || std::abs(xy.x2() - m_cursor->y()) < std::abs(nextY - m_cursor->y())) {
          nextCurveIndex = i;
          xDelta = currentXDelta;
          nextY = xy.x2();
          nextT = potentialNextT;
        }
      }
    }
  }
  if (nextCurveIndex < 0) {
    return;
  }
  m_cursor->moveTo(nextT, nextT, nextY);
  selectFunctionWithCursor(nextCurveIndex);
  return;
}

}
