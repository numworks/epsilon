#include "graph_controller.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>
#include <poincare/layout_helper.h>
#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Graph {

GraphController::GraphController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, ButtonRowController * header) :
  FunctionGraphController(parentResponder, inputEventHandlerDelegate, header, curveViewRange, &m_view, cursor, indexFunctionSelectedByCursor),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_interestView(&m_view),
  m_view(curveViewRange, m_cursor, &m_bannerView, &m_cursorView, &m_interestView),
  m_graphRange(curveViewRange),
  m_curveParameterController(inputEventHandlerDelegate, curveViewRange, &m_bannerView, m_cursor, &m_view, this),
  m_functionSelectionController(this)
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
  m_cursorView.resetMemoization();
  m_interestView.setInterest(Solver<double>::Interest::None);
  m_view.setCursorView(&m_cursorView);
  FunctionGraphController::viewWillAppear();
  selectFunctionWithCursor(indexFunctionSelectedByCursor(), true);
}

void GraphController::didBecomeFirstResponder() {
  FunctionGraphController::didBecomeFirstResponder();
  m_view.selectRecord(functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor()));
}

template <typename T> Coordinate2D<T> evaluator(T t, const void * model, Context * context) {
  const ContinuousFunction * f = static_cast<const ContinuousFunction *>(model);
  return f->evaluateXYAtParameter(t, context);
}
template <typename T> Coordinate2D<T> evaluatorSecondCurve(T t, const void * model, Context * context) {
  const ContinuousFunction * f = static_cast<const ContinuousFunction *>(model);
  return f->evaluateXYAtParameter(t, context, 1);
}

Range2D GraphController::optimalRange(bool computeX, bool computeY, Range2D originalRange) const {
  Context * context = App::app()->localContext();
  Zoom zoom(NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(), context, k_maxFloat);
  ContinuousFunctionStore * store = functionStore();
  int nbFunctions = store->numberOfActiveFunctions();
  Range1D xBounds = computeX ? Range1D(-k_maxFloat, k_maxFloat) : *originalRange.x();
  Range1D yBounds = computeY ? Range1D(-k_maxFloat, k_maxFloat) : *originalRange.y();
  Range2D forcedRange = Range2D(computeX ? Range1D() : *originalRange.x(), computeY ? Range1D() : *originalRange.y());
  zoom.setForcedRange(forcedRange);

  for (int i = 0; i < nbFunctions; i++) {
    ExpiringPointer<ContinuousFunction> f = store->modelForRecord(store->activeRecordAtIndex(i));
    if (f->basedOnCostlyAlgorithms(context)) {
      continue;
    }
    if (f->properties().isPolar() || f->properties().isParametric()) {
      assert(std::isfinite(f->tMin()) && std::isfinite(f->tMax()));
      zoom.setBounds(f->tMin(), f->tMax());
      zoom.fitFullFunction(evaluator<float>, f.operator->());
    } else {
      assert(f->properties().isCartesian());
      bool alongY = f->isAlongY();
      Range1D * bounds = alongY ? &yBounds : &xBounds;
      zoom.setBounds(bounds->min(), bounds->max());
      zoom.fitPointsOfInterest(evaluator<float>, f.operator->(), alongY, evaluator<double>);
      if (f->numberOfSubCurves() > 1) {
        assert(f->numberOfSubCurves() == 2);
        zoom.fitPointsOfInterest(evaluatorSecondCurve<float>, f.operator->(), alongY, evaluatorSecondCurve<double>);
      }
      if (f->properties().canComputeIntersectionsWithFunctionsAlongSameVariable()) {
        ContinuousFunction * mainF = f.operator->();
        for (int j = i + 1; j < nbFunctions; j++) {
          ExpiringPointer<ContinuousFunction> g = store->modelForRecord(store->activeRecordAtIndex(j));
          if (!g->properties().canComputeIntersectionsWithFunctionsAlongSameVariable() || g->isAlongY() != alongY) {
            continue;
          }
          zoom.fitIntersections(evaluator<float>, mainF, evaluator<float>, g.operator->());
        }
      }
    }
  }

  if (computeY) {
    zoom.setBounds(xBounds.min(), xBounds.max());
    for (int i = 0; i < nbFunctions; i++) {
      ExpiringPointer<ContinuousFunction> f = store->modelForRecord(store->activeRecordAtIndex(i));
      if (f->basedOnCostlyAlgorithms(context) || !f->properties().isCartesian()) {
        continue;
      }
      bool alongY = f->isAlongY();
      zoom.fitMagnitude(evaluator, f.operator->(), alongY);
      if (f->numberOfSubCurves() > 1) {
        zoom.fitMagnitude(evaluatorSecondCurve, f.operator->(), alongY);
      }
    }
  }

  Range2D newRange = zoom.range(true, defaultRangeIsNormalized());
  return Range2D(*(computeX ? newRange : originalRange).x(), *(computeY ? newRange : originalRange).y());
}

PointsOfInterestCache * GraphController::pointsOfInterestForRecord(Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
  PointsOfInterestCache * cache = nullptr;
  for (int i = 0; i < static_cast<int>(m_pointsOfInterest.length()); i++) {
    if (m_pointsOfInterest.elementAtIndex(i)->record() == record) {
      cache = m_pointsOfInterest.elementAtIndex(i);
      break;
    }
  }

  if (!cache) {
    PointsOfInterestCache newCache(record);
    m_pointsOfInterest.push(newCache);
    cache = m_pointsOfInterest.elementAtIndex(m_pointsOfInterest.length() - 1);
  }

  // Set bounds of cache
  if (f->isAlongY()) {
    cache->setBounds(m_graphRange->yMin(), m_graphRange->yMax());
  } else {
    float tMin = f->tMin();
    float tMax = f->tMax();
    cache->setBounds(std::clamp(m_graphRange->xMin(), tMin, tMax), std::clamp(m_graphRange->xMax(), tMin, tMax));
  }

  return cache;
}

Layout GraphController::FunctionSelectionController::nameLayoutAtIndex(int j) const {
  GraphController * graphController = static_cast<GraphController *>(m_graphController);
  ContinuousFunctionStore * store = graphController->functionStore();
  ExpiringPointer<ContinuousFunction> function = store->modelForRecord(store->activeRecordAtIndex(j));
  constexpr size_t bufferSize = ContinuousFunction::k_maxNameWithArgumentSize;
  char buffer[bufferSize];
  int size = function->nameWithArgument(buffer, bufferSize);
  return LayoutHelper::String(buffer, size);
}

void GraphController::selectFunctionWithCursor(int functionIndex, bool willBeVisible) {
  FunctionGraphController::selectFunctionWithCursor(functionIndex, willBeVisible);
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(functionIndex);
  ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(record);
  m_cursorView.setColor(f->color());
  m_view.selectRecord(record);

  // Reload for points of interest
  if (willBeVisible) {
    m_view.reload(false, true);
  }
}

bool GraphController::displayDerivativeInBanner() const {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  return functionStore()->modelForRecord(record)->displayDerivative() &&
    functionStore()->modelForRecord(record)->canDisplayDerivative();
}

 void GraphController::reloadBannerView() {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  bool displayDerivative = displayDerivativeInBanner();
  m_bannerView.setDisplayParameters(true, displayDerivative, false);
  FunctionGraphController::reloadBannerView();
  if (!displayDerivative) {
    return;
  }
  reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, record);
}

bool GraphController::moveCursorHorizontally(int direction, int scrollSpeed) {
  Ion::Storage::Record record = functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor());
  // Redraw the points of interest in case the computation was interrupted
  m_interestView.dirty();
  return privateMoveCursorHorizontally(m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit, record, m_view.pixelWidth(), scrollSpeed, &m_selectedSubCurveIndex);
}

int GraphController::nextCurveIndexVertically(bool goingUp, int currentSelectedCurve, Poincare::Context * context, int currentSubCurveIndex, int * nextSubCurveIndex) const {
  assert(nextSubCurveIndex != nullptr);
  int nbOfActiveFunctions = 0;
  if (!functionStore()->displaysNonCartesianFunctions(&nbOfActiveFunctions)) {
    return FunctionGraphController::nextCurveIndexVertically(goingUp, currentSelectedCurve, context, currentSubCurveIndex, nextSubCurveIndex);
  }
  // Handle for sub curve in current function
  if (!goingUp) {
    ExpiringPointer<ContinuousFunction> currentF = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(currentSelectedCurve));
    if (currentF->numberOfSubCurves() > currentSubCurveIndex + 1) {
      // Switch to next sub curve
      *nextSubCurveIndex = currentSubCurveIndex + 1;
      return currentSelectedCurve;
    }
  } else if (goingUp && currentSubCurveIndex > 0) {
    // Switch to previous sub curve
    *nextSubCurveIndex = currentSubCurveIndex - 1;
    return currentSelectedCurve;
  }
  // Go to the next function
  int nextActiveFunctionIndex = currentSelectedCurve + (goingUp ? -1 : 1);
  if (nextActiveFunctionIndex >= nbOfActiveFunctions || nextActiveFunctionIndex < 0) {
    return -1;
  }
  if (goingUp) {
    // Select last sub curve in next function when going up
    ExpiringPointer<ContinuousFunction> nextF = functionStore()->modelForRecord(functionStore()->activeRecordAtIndex(nextActiveFunctionIndex));
    *nextSubCurveIndex = nextF->numberOfSubCurves() - 1;
  } else {
    // Select first sub curve in next function
    *nextSubCurveIndex = 0;
  }
  return nextActiveFunctionIndex;
}

double GraphController::defaultCursorT(Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
  if (function->properties().isCartesian()) {
    return FunctionGraphController::defaultCursorT(record);
  }
  return function->tMin();
}

void GraphController::jumpToLeftRightCurve(double t, int direction, int functionsCount, Ion::Storage::Record record) {
  if (functionsCount == 1) {
    return;
  }
  int nextCurveIndex = -1;
  double xDelta = DBL_MAX;
  double nextY = 0.0;
  double nextT = 0.0;
  int nextSubCurve = 0;
  for (int i = 0; i < functionsCount; i++) {
    Ion::Storage::Record currentRecord = functionStore()->activeRecordAtIndex(i);
    if (currentRecord == record) {
      continue;
    }
    ExpiringPointer<ContinuousFunction> f = functionStore()->modelForRecord(currentRecord);
    assert(f->properties().isCartesian());
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
        // If a function has two curves
        for (int subCurveIndex = 0; subCurveIndex < f->numberOfSubCurves(); subCurveIndex++) {
          Coordinate2D<double> xy = f->evaluateXYAtParameter(potentialNextT, App::app()->localContext(), subCurveIndex);
          if (currentXDelta < xDelta || std::abs(xy.x2() - m_cursor->y()) < std::abs(nextY - m_cursor->y())) {
            nextCurveIndex = i;
            xDelta = currentXDelta;
            nextY = xy.x2();
            nextT = potentialNextT;
            nextSubCurve = subCurveIndex;
          }
        }
      }
    }
  }
  if (nextCurveIndex < 0) {
    return;
  }
  m_cursor->moveTo(nextT, nextT, nextY);
  m_selectedSubCurveIndex = nextSubCurve;
  selectFunctionWithCursor(nextCurveIndex, true);
  return;
}

bool GraphController::moveCursorVertically(int direction) {
  bannerView()->setInterestMessage(I18n::Message::Default, graphView()->cursorView());
  bool moved = FunctionGraphController::moveCursorVertically(direction);
  if (!moved) {
    return false;
  }
  constexpr static int k_snapStep = 100;
  double t = m_cursor->t();
  double dt = (m_graphRange->xMax() - m_graphRange->xMin()) / k_snapStep;
  if (moved) {
    snapToInterestAndUpdateBannerAndCursor(m_cursor, t - dt, t + dt);
  }
  return true;
}

void GraphController::moveCursorAndCenterIfNeeded(double t) {
  bannerView()->setInterestMessage(I18n::Message::Default, graphView()->cursorView());
  FunctionGraphController::moveCursorAndCenterIfNeeded(t);
  if (snapToInterestAndUpdateBannerAndCursor(m_cursor, std::nextafter(m_cursor->t(), -static_cast<double>(INFINITY)), std::nextafter(m_cursor->t(), static_cast<double>(INFINITY)))) {
    reloadBannerView();
  }
}

}
