#include "graph_controller.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/layout.h>
#include <poincare/numeric/zoom.h>

#include <algorithm>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

GraphController::GraphController(
    Escher::Responder* parentResponder, Escher::ButtonRowController* header,
    Shared::InteractiveCurveViewRange* interactiveRange,
    CurveViewCursor* cursor, int* selectedCurveIndex,
    FunctionParameterController* functionParameterController,
    DerivativeColumnParameterController* derivativeColumnParameterController)
    : FunctionGraphController(parentResponder, header, interactiveRange,
                              &m_view, cursor, selectedCurveIndex),
      m_bannerView(this, this),
      m_view(interactiveRange, m_cursor, &m_bannerView, &m_cursorView),
      m_graphRange(interactiveRange),
      m_curveParameterController(interactiveRange, &m_bannerView, m_cursor,
                                 &m_view, this, functionParameterController,
                                 derivativeColumnParameterController),
      m_functionSelectionController(this) {
  m_graphRange->setDelegate(this);
  showGridTypeMenu(true);
}

I18n::Message GraphController::emptyMessage() {
  if (functionStore()->numberOfDefinedModels() == 0) {
    return I18n::Message::NoFunction;
  }
  return I18n::Message::NoActivatedFunction;
}

void GraphController::viewWillAppear() {
  m_view.setTangentDisplay(false);
  m_view.setInterest(Solver<double>::Interest::None);
  m_cursorView.resetMemoization();
  m_view.setCursorView(&m_cursorView);
  FunctionGraphController::viewWillAppear();
}

void GraphController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  FunctionGraphController::handleResponderChainEvent(event);
  if (event.type == ResponderChainEventType::BecameFirst) {
    m_view.selectRecord(recordAtSelectedCurveIndex());
  }
}

bool GraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Idle) {
    // Compute the points of interest when the user is not active
    m_view.resumePointsOfInterestDrawing();
    return true;
  }
  return Shared::FunctionGraphController::handleEvent(event);
}

struct ContinuousFunctionAndContext {
  const ContinuousFunction* func;
  Context* ctx;
};

template <typename T>
static Coordinate2D<T> evaluator(T t, const void* model) {
  const ContinuousFunctionAndContext curve =
      *static_cast<const ContinuousFunctionAndContext*>(model);
  return curve.func->evaluateXYAtParameter(t, curve.ctx);
}
template <typename T>
static Coordinate2D<T> evaluatorSecondCurve(T t, const void* model) {
  const ContinuousFunctionAndContext curve =
      *static_cast<const ContinuousFunctionAndContext*>(model);
  return curve.func->evaluateXYAtParameter(t, curve.ctx, 1);
}
template <typename T, int coordinate>
static Coordinate2D<T> parametricExpressionEvaluator(T t, const void* model) {
  const SystemFunctionPoint* e = static_cast<const SystemFunctionPoint*>(model);
  assert(e->isPoint());
  assert(coordinate == 0 || coordinate == 1);
  // TODO: Approximating the other coordinate could be skipped for performances.
  Coordinate2D<T> value =
      e->approximateToPointOrScalarWithValue<T>(t).toPoint();
  return Coordinate2D<T>(t, (coordinate == 0) ? value.x() : value.y());
}

Range2D<float> GraphController::optimalRange(
    bool computeX, bool computeY, Range2D<float> originalRange) const {
  Context* context = App::app()->localContext();
  Zoom zoom(NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
  ContinuousFunctionStore* store = functionStore();
  if (store->memoizationOverflows()) {
    /* Do not compute autozoom if store is full because the computation is too
     * slow. */
    Range1D<float> xRange(-Range1D<float>::k_defaultHalfLength,
                          Range1D<float>::k_defaultHalfLength);
    Range2D<float> defaultRange(xRange, xRange);
    defaultRange.setRatio(InteractiveCurveViewRange::NormalYXRatio(), true,
                          k_maxFloat);
    return Range2D<float>(*(computeX ? defaultRange : originalRange).x(),
                          *(computeY ? defaultRange : originalRange).y());
  }
  bool canComputeIntersections
      [ContinuousFunctionStore::k_maxNumberOfMemoizedModels];
  int nbFunctions = store->numberOfActiveFunctions();
  assert(nbFunctions <= ContinuousFunctionStore::k_maxNumberOfMemoizedModels);
  Range1D<float> xBounds =
      computeX ? Range1D<float>(-k_maxFloat, k_maxFloat) : *originalRange.x();
  Range1D<float> yBounds =
      computeY ? Range1D<float>(-k_maxFloat, k_maxFloat) : *originalRange.y();
  Range2D<float> forcedRange =
      Range2D<float>(computeX ? Range1D<float>() : *originalRange.x(),
                     computeY ? Range1D<float>() : *originalRange.y());
  zoom.setForcedRange(forcedRange);

  for (int i = 0; i < nbFunctions; i++) {
    canComputeIntersections[i] = false;
    ExpiringPointer<const ContinuousFunction> f =
        store->constModelForRecord(store->activeRecordAtIndex(i));
    ContinuousFunctionAndContext fModel{.func = f.operator->(), .ctx = context};
    if (f->approximationBasedOnCostlyAlgorithms(context)) {
      continue;
    }
    if (f->properties().isPolar() || f->properties().isInversePolar() ||
        f->properties().isParametric()) {
      assert(std::isfinite(f->tMin()) && std::isfinite(f->tMax()));
      SystemFunctionPoint e = f->parametricForm(context).getSystemFunction(
          Shared::Function::k_unknownName);
      // Compute the ordinate range of x(t) and y(t)
      Range1D<float> ranges[2];
      Zoom::Function2D<float> floatEvaluators[2] = {
          parametricExpressionEvaluator<float, 0>,
          parametricExpressionEvaluator<float, 1>};
      Zoom::Function2D<double> doubleEvaluators[2] = {
          parametricExpressionEvaluator<double, 0>,
          parametricExpressionEvaluator<double, 1>};
      for (int coordinate = 0; coordinate < 2; coordinate++) {
        Zoom zoomAlongCoordinate(
            NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
        zoomAlongCoordinate.setBounds(f->tMin(), f->tMax());
        zoomAlongCoordinate.fitPointsOfInterest(floatEvaluators[coordinate], &e,
                                                false,
                                                doubleEvaluators[coordinate]);
        zoomAlongCoordinate.fitBounds(floatEvaluators[coordinate], &e, false);
        ranges[coordinate] = *zoomAlongCoordinate.range(false, false).y();
      }

      // Fit the zoom to the range of x(t) and y(t)
      zoom.fitPoint(Coordinate2D<float>(ranges[0].max(), ranges[1].max()));
      zoom.fitPoint(Coordinate2D<float>(ranges[0].min(), ranges[1].min()));
    } else if (f->properties().isScatterPlot()) {
      for (Coordinate2D<float> p : f->iterateScatterPlot(context)) {
        zoom.fitPoint(p);
      }
    } else {
      assert(f->properties().isCartesian());
      canComputeIntersections[i] = true;
      bool alongY = f->isAlongY();
      Range1D<float>* bounds = alongY ? &yBounds : &xBounds;
      // Use the intersection between the definition domain of f and the bounds
      zoom.setBounds(std::clamp(f->tMin(), bounds->min(), bounds->max()),
                     std::clamp(f->tMax(), bounds->min(), bounds->max()));
      zoom.fitPointsOfInterest(evaluator<float>, &fModel, alongY,
                               evaluator<double>, canComputeIntersections + i);
      zoom.fitBounds(evaluator<float>, &fModel, alongY);
      if (f->numberOfSubCurves() > 1) {
        assert(f->numberOfSubCurves() == 2);
        zoom.fitPointsOfInterest(evaluatorSecondCurve<float>, &fModel, alongY,
                                 evaluatorSecondCurve<double>,
                                 canComputeIntersections + i);
        zoom.fitBounds(evaluatorSecondCurve<float>, &fModel, alongY);
      }

      /* Special case for piecewise functions: we want to display the branch
       * edges even if the behaviour is not "interesting".
       * FIXME For simplicity's sake, only the first piecewise operator will
       * have its conditions fitted. It is assumed that expressions containing
       * more than one piecewise will be rare. */
      const Internal::Tree* piecewise =
          f->expressionApproximated(context).tree()->firstDescendantSatisfying(
              [](const Internal::Tree* t) { return t->isPiecewise(); });
      if (piecewise) {
        zoom.fitConditions(SystemFunction::Builder(piecewise), evaluator<float>,
                           &fModel, alongY);
      }

      if (canComputeIntersections[i] &&
          f->properties()
              .canComputeIntersectionsWithFunctionsAlongSameVariable()) {
        for (int j = 0; j < i; j++) {
          ExpiringPointer<const ContinuousFunction> g =
              store->constModelForRecord(store->activeRecordAtIndex(j));
          if (canComputeIntersections[j] &&
              g->properties()
                  .canComputeIntersectionsWithFunctionsAlongSameVariable() &&
              g->isAlongY() == alongY &&
              !g->approximationBasedOnCostlyAlgorithms(context)) {
            ContinuousFunctionAndContext gModel{.func = g.operator->(),
                                                .ctx = context};
            zoom.fitIntersections(evaluator<float>, &fModel, evaluator<float>,
                                  &gModel);
          }
        }
      }
    }
  }

  if (computeY) {
    zoom.setBounds(xBounds.min(), xBounds.max());
    for (int i = 0; i < nbFunctions; i++) {
      ExpiringPointer<ContinuousFunction> f =
          store->modelForRecord(store->activeRecordAtIndex(i));
      if (f->approximationBasedOnCostlyAlgorithms(context) ||
          !f->properties().isCartesian()) {
        continue;
      }
      bool alongY = f->isAlongY();
      /* If X range is forced (computeX is false), we don't want to crop the Y
       * axis: we want to see the value of f at each point of X range. */
      bool cropOutliers = computeX;
      ContinuousFunctionAndContext model{.func = f.operator->(),
                                         .ctx = context};
      zoom.fitMagnitude(evaluator, &model, cropOutliers, alongY);
      if (f->numberOfSubCurves() > 1) {
        zoom.fitMagnitude(evaluatorSecondCurve, &model, cropOutliers, alongY);
      }
    }
  }

  Range2D<float> newRange = zoom.range(true, defaultRangeIsNormalized());
  return Range2D<float>(*(computeX ? newRange : originalRange).x(),
                        *(computeY ? newRange : originalRange).y());
}

PointsOfInterestCache* GraphController::pointsOfInterestForRecord(
    Ion::Storage::Record record) {
  ExpiringPointer<ContinuousFunction> f =
      functionStore()->modelForRecord(record);
  PointsOfInterestCache* cache = nullptr;
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
    cache->setBounds(std::clamp(m_graphRange->xMin(), tMin, tMax),
                     std::clamp(m_graphRange->xMax(), tMin, tMax));
  }

  return cache;
}

const Layout GraphController::FunctionSelectionController::nameLayoutAtIndex(
    int j) const {
  GraphController* graphController =
      static_cast<GraphController*>(m_graphController);
  ContinuousFunctionStore* store = graphController->functionStore();
  ExpiringPointer<ContinuousFunction> function =
      store->modelForRecord(store->activeRecordAtIndex(j));
  return function->layout();
}

void GraphController::reloadBannerView() {
  Ion::Storage::Record record = recordAtSelectedCurveIndex();
  ExpiringPointer<ContinuousFunction> f =
      functionStore()->modelForRecord(record);
  int derivationOrder =
      f->derivationOrderFromSubCurveIndex(m_selectedSubCurveIndex);
  bool displayOrdinate, displayValueFirstDerivative,
      displayValueSecondDerivative;
  f->valuesToDisplayOnDerivativeCurve(derivationOrder, &displayOrdinate,
                                      &displayValueFirstDerivative,
                                      &displayValueSecondDerivative);
  m_bannerView.setDisplayParameters(
      {.showInterest = true,
       .showOrdinate = displayOrdinate,
       .showFirstDerivative = displayValueFirstDerivative,
       .showSecondDerivative = displayValueSecondDerivative});
  FunctionGraphController::reloadBannerView();
  if (displayValueFirstDerivative) {
    reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, record, 1);
  }
  if (displayValueSecondDerivative) {
    reloadDerivativeInBannerViewForCursorOnFunction(m_cursor, record, 2);
  }
}

bool GraphController::moveCursorHorizontally(OMG::HorizontalDirection direction,
                                             int scrollSpeed) {
  assert(m_selectedSubCurveIndex < numberOfSubCurves(*m_selectedCurveIndex));
  Ion::Storage::Record record = recordAtSelectedCurveIndex();
  return privateMoveCursorHorizontally(
      m_cursor, direction, m_graphRange, k_numberOfCursorStepsInGradUnit,
      record, m_view.pixelWidth(), scrollSpeed, &m_selectedSubCurveIndex);
}

void GraphController::selectCurveAtIndex(int curveIndex, bool willBeVisible,
                                         int subCurveIndex) {
  m_cursorView.setIsRing(functionStore()
                             ->modelForRecord(recordAtCurveIndex(curveIndex))
                             ->properties()
                             .isScatterPlot());
  FunctionGraphController::selectCurveAtIndex(curveIndex, willBeVisible,
                                              subCurveIndex);
}

int GraphController::nextCurveIndexVertically(OMG::VerticalDirection direction,
                                              int currentCurveIndex,
                                              Poincare::Context* context,
                                              int currentSubCurveIndex,
                                              int* nextSubCurveIndex) const {
  assert(nextSubCurveIndex != nullptr);
  int nbOfActiveFunctions = 0;
  if (functionStore()->displaysOnlyCartesianFunctions(&nbOfActiveFunctions)) {
    return FunctionGraphController::nextCurveIndexVertically(
        direction, currentCurveIndex, context, currentSubCurveIndex,
        nextSubCurveIndex);
  }
  // Handle for sub curve in current function
  if (direction.isDown()) {
    if (numberOfSubCurves(currentCurveIndex) > currentSubCurveIndex + 1) {
      // Switch to next sub curve
      *nextSubCurveIndex = currentSubCurveIndex + 1;
      return currentCurveIndex;
    }
  } else if (direction.isUp() && currentSubCurveIndex > 0) {
    // Switch to previous sub curve
    *nextSubCurveIndex = currentSubCurveIndex - 1;
    return currentCurveIndex;
  }
  // Go to the next function
  int nextActiveFunctionIndex = currentCurveIndex + (direction.isUp() ? -1 : 1);
  if (nextActiveFunctionIndex >= nbOfActiveFunctions ||
      nextActiveFunctionIndex < 0) {
    return -1;
  }
  if (direction.isUp()) {
    // Select last sub curve in next function when going up
    *nextSubCurveIndex = numberOfSubCurves(nextActiveFunctionIndex) - 1;
  } else {
    // Select first sub curve in next function
    *nextSubCurveIndex = 0;
  }
  return nextActiveFunctionIndex;
}

double GraphController::defaultCursorT(Ion::Storage::Record record,
                                       bool ignoreMargins) {
  ExpiringPointer<ContinuousFunction> function =
      functionStore()->modelForRecord(record);
  if (function->properties().isCartesian()) {
    return FunctionGraphController::defaultCursorT(record, ignoreMargins);
  }

  Poincare::Context* context = App::app()->localContext();
  if (function->properties().isScatterPlot()) {
    float t = 0;
    for (Coordinate2D<float> p : function->iterateScatterPlot(context)) {
      if (isCursorVisibleAtPosition(p, ignoreMargins)) {
        return t;
      }
      ++t;
    }
    return 0.;
  }

  assert(function->properties().isParametric() ||
         function->properties().isPolar() ||
         function->properties().isInversePolar());
  float tMin = function->tMin(), tMax = function->tMax();
  float tRange = tMax - tMin;
  constexpr int numberOfRangeSubdivisions = 4;

  float currentT;
  Coordinate2D<float> currentXY;
  for (int i = 0; i <= numberOfRangeSubdivisions; i++) {
    /* Start from tMin and place the cursor on each fourth of the interval
     * until a t where the cursor is visible is found.
     * currentT values will be tMin / tMin+0.25*tRange / tMin+0.5*tRange /
     * tMin+0.75*tRange / tMax */
    currentT = tMin + i * (tRange / numberOfRangeSubdivisions);
    // Using first subCurve for default cursor.
    currentXY = function->evaluateXYAtParameter(currentT, context, 0);
    if (isCursorVisibleAtPosition(currentXY, ignoreMargins)) {
      break;
    }
  }

  if (!isCursorVisibleAtPosition(currentXY, ignoreMargins)) {
    // If no positions make the cursor visible, return the min value
    currentT = tMin;
  }

  return currentT;
}

void GraphController::openMenuForSelectedCurve() {
  Ion::Storage::Record record = recordAtSelectedCurveIndex();
  ExpiringPointer<ContinuousFunction> f =
      functionStore()->modelForRecord(record);
  int derivationOrder =
      f->derivationOrderFromSubCurveIndex(m_selectedSubCurveIndex);
  m_curveParameterController.setRecord(record, derivationOrder);
  stackController()->push(&m_curveParameterController);
}

bool GraphController::moveCursorVertically(OMG::VerticalDirection direction) {
  int currentActiveFunctionIndex = *m_selectedCurveIndex;
  Context* context = App::app()->localContext();

  int nextSubCurve = 0;
  int nextCurve =
      nextCurveIndexVertically(direction, currentActiveFunctionIndex, context,
                               m_selectedSubCurveIndex, &nextSubCurve);
  if (nextCurve < 0) {
    return false;
  }

  ExpiringPointer<ContinuousFunction> currentF =
      functionStore()->modelForRecord(recordAtSelectedCurveIndex());
  float nextT =
      currentF->properties().isScatterPlot() && std::isfinite(m_cursor->x())
          ? m_cursor->x()
          : m_cursor->t();

  ExpiringPointer<ContinuousFunction> nextF =
      functionStore()->modelForRecord(recordAtCurveIndex(nextCurve));
  if (nextF->properties().isScatterPlot()) {
    double nextX = nextT;
    nextT = -1;
    double previousX = -INFINITY;
    for (Coordinate2D<float> xy : nextF->iterateScatterPlot(context)) {
      if (xy.x() >= nextX) {
        if (xy.x() - nextX < nextX - previousX) {
          ++nextT;
        }
        break;
      }
      ++nextT;
      previousX = xy.x();
    }
  }

  moveCursorVerticallyToPosition(nextCurve, nextSubCurve, nextT);
  return true;
}

void GraphController::jumpToLeftRightCurve(double t,
                                           OMG::HorizontalDirection direction,
                                           int functionsCount,
                                           Ion::Storage::Record record) {
  if (functionsCount == 1) {
    return;
  }
  int nextCurveIndex = -1;
  double xDelta = DBL_MAX;
  double nextY = 0.0;
  double nextT = 0.0;
  int nextSubCurve = 0;
  for (int i = 0; i < functionsCount; i++) {
    Ion::Storage::Record currentRecord = recordAtCurveIndex(i);
    if (currentRecord == record) {
      continue;
    }
    ExpiringPointer<ContinuousFunction> f =
        functionStore()->modelForRecord(currentRecord);
    assert(f->properties().isCartesian());
    /* Select the closest horizontal curve, then the closest vertically, then
     * the lowest curve index. */
    double currentTMin = f->tMin();
    double currentTMax = f->tMax();
    assert(!std::isnan(currentTMin));
    assert(!std::isnan(currentTMax));
    if ((direction.isRight() && currentTMax > t) ||
        (direction.isLeft() && currentTMin < t)) {
      double currentXDelta = direction.isRight()
                                 ? (t >= currentTMin ? 0.0 : currentTMin - t)
                                 : (t <= currentTMax ? 0.0 : t - currentTMax);
      assert(currentXDelta >= 0.0);
      if (currentXDelta <= xDelta) {
        double potentialNextTMin = f->tMin();
        double potentialNextTMax = f->tMax();
        double potentialNextT =
            std::max(potentialNextTMin, std::min(potentialNextTMax, t));
        // If a function has two curves
        for (int subCurveIndex = 0; subCurveIndex < f->numberOfSubCurves();
             subCurveIndex++) {
          Coordinate2D<double> xy = f->evaluateXYAtParameter(
              potentialNextT, App::app()->localContext(), subCurveIndex);
          if (currentXDelta < xDelta || std::abs(xy.y() - m_cursor->y()) <
                                            std::abs(nextY - m_cursor->y())) {
            nextCurveIndex = i;
            xDelta = currentXDelta;
            nextY = xy.y();
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
  selectCurveAtIndex(nextCurveIndex, true);
  return;
}

void GraphController::reloadBannerViewForCursorOnFunction(
    double cursorT, double cursorX, double cursorY, Ion::Storage::Record record,
    FunctionStore* functionStore, Poincare::Context* context,
    bool cappedNumberOfSignificantDigits) {
  ExpiringPointer<ContinuousFunction> function =
      App::app()->functionStore()->modelForRecord(record);
  PointsOfInterestCache* pointsOfInterest = pointsOfInterestForRecord(record);
  bannerView()->emptyInterestMessages(&m_cursorView);
  /* The interests are sorted from most important to lowest, in case there is
   * not enough space on the banner to display all of them. */
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::LocalMinimum)) {
    assert(!function->isAlongY());
    bannerView()->addInterestMessage(I18n::Message::Minimum, &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::LocalMaximum)) {
    assert(!function->isAlongY());
    bannerView()->addInterestMessage(I18n::Message::Maximum, &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::Intersection)) {
    bannerView()->addInterestMessage(I18n::Message::Intersection,
                                     &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::Root)) {
    bannerView()->addInterestMessage(
        function->isAlongY() ? I18n::Message::LineYInterceptDescription
                             : I18n::Message::Zero,
        &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::YIntercept)) {
    bannerView()->addInterestMessage(
        function->isAlongY() ? I18n::Message::Zero
                             : I18n::Message::LineYInterceptDescription,
        &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::UnreachedDiscontinuity)) {
    // Display undef in banner view
    cursorY = NAN;
    bannerView()->addInterestMessage(I18n::Message::Discontinuity,
                                     &m_cursorView);
  }

  /* Cap number of significant digits for point of interest to be consistent
   * with CalculationGraphController */
  cappedNumberOfSignificantDigits =
      cappedNumberOfSignificantDigits ||
      bannerView()->numberOfInterestMessages() > 0;
  FunctionGraphController::reloadBannerViewForCursorOnFunction(
      cursorT, cursorX, cursorY, record, functionStore, context,
      cappedNumberOfSignificantDigits);
}

}  // namespace Graph
