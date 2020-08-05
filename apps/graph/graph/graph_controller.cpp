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

bool GraphController::defaultRangeIsNormalized() const {
  return functionStore()->displaysNonCartesianFunctions();
}

void GraphController::interestingRanges(InteractiveCurveViewRange * range) const {
  privateComputeRanges(true, range);
}

Shared::InteractiveCurveViewRangeDelegate::Range GraphController::computeYRange(Shared::InteractiveCurveViewRange * interactiveCurveViewRange) {
  InteractiveCurveViewRange tempRange = *interactiveCurveViewRange;
  tempRange.setYAuto(false);
  privateComputeRanges(false, &tempRange);
  return Shared::InteractiveCurveViewRangeDelegate::Range{.min = tempRange.yMin(), .max = tempRange.yMax()};
}

void GraphController::privateComputeRanges(bool tuneXRange, InteractiveCurveViewRange * range) const {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  float resultXMin = tuneXRange ? FLT_MAX : range->xMin();
  float resultXMax = tuneXRange ? -FLT_MAX : range->xMax();
  float resultYMin = FLT_MAX;
  float resultYMax = -FLT_MAX;
  assert(functionStore()->numberOfActiveFunctions() > 0);
  int functionsCount = functionStore()->numberOfActiveFunctions();
  for (int i = 0; i < functionsCount; i++) {
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    f->rangeForDisplay(&resultXMin, &resultXMax, &resultYMin, &resultYMax, context, tuneXRange);
  }

  range->setXMin(resultXMin);
  range->setXMax(resultXMax);
  range->setYMin(resultYMin);
  range->setYMax(resultYMax);
  /* We can only call this method once the X range has been fully computed. */
  yRangeForCursorFirstMove(range);
}

void GraphController::yRangeForCursorFirstMove(InteractiveCurveViewRange * range) const {
  Poincare::Context * context = textFieldDelegateApp()->localContext();
  assert(functionStore()->numberOfActiveFunctions() > 0);
  int functionsCount = functionStore()->numberOfActiveFunctions();

  float cursorStep = range->xGridUnit() / k_numberOfCursorStepsInGradUnit;
  float yN, yP;

  for (int i = 0; i < functionsCount; i++) {
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(i));
    if (f->plotType() != ContinuousFunction::PlotType::Cartesian) {
      continue;
    }
    yN = f->evaluateXYAtParameter(range->xCenter() - cursorStep, context).x2();
    yP = f->evaluateXYAtParameter(range->xCenter() + cursorStep, context).x2();
    range->setYMin(std::min(range->yMin(), std::min(yN, yP)));
    range->setYMax(std::max(range->yMax(), std::max(yN, yP)));
  }
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

bool GraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, record, scrollSpeed);
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
