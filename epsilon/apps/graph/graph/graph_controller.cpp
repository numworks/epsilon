#include "graph_controller.h"

#include <apps/shared/global_store.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/layout.h>

#include <algorithm>

#include "../app.h"
#include "optimal_range.h"

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
  if (functionOrSequenceContext().numberOfDefinedModels() == 0) {
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
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
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

Poincare::Range2D<float> GraphController::optimalRange(
    bool computeX, bool computeY,
    Poincare::Range2D<float> originalRange) const {
  return OptimalRange(computeX, computeY, originalRange,
                      &functionOrSequenceContext(),
                      m_graphRange->gridType() ==
                          Shared::InteractiveCurveViewRange::GridType::Polar,
                      App::app()->localContext());
}

PointsOfInterestCache* GraphController::pointsOfInterestForRecord(
    Ion::Storage::Record record) {
  OMG::ExpiringPointer<const ContinuousFunction> f =
      functionOrSequenceContext().modelForRecord(record);
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
  const ContinuousFunctionContext& functionContext =
      graphController->functionOrSequenceContext();
  OMG::ExpiringPointer<const ContinuousFunction> function =
      functionContext.modelForRecord(functionContext.activeRecordAtIndex(j));
  return function->layout();
}

void GraphController::reloadBannerView() {
  Ion::Storage::Record record = recordAtSelectedCurveIndex();
  OMG::ExpiringPointer<const ContinuousFunction> f =
      functionOrSequenceContext().modelForRecord(record);
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
  Ion::Storage::Record record = recordAtCurveIndex(curveIndex);
  ContinuousFunctionProperties properties =
      functionOrSequenceContext().modelForRecord(record)->properties();
  bool cursorShouldBeRing =
      properties.isScatterPlot() ||
      pointsOfInterestForRecord(record)
          ->hasDisplayableUnreachedInterestAtCoordinates(m_cursor->x(),
                                                         m_cursor->y()) ||
      (properties.isStrictInequality() &&
       pointsOfInterestForRecord(record)->hasDisplayableInterestAtCoordinates(
           m_cursor->x(), m_cursor->y()));
  setCursorIsRing(cursorShouldBeRing);
  FunctionGraphController::selectCurveAtIndex(curveIndex, willBeVisible,
                                              subCurveIndex);
}

int GraphController::nextCurveIndexVertically(OMG::VerticalDirection direction,
                                              int currentCurveIndex,
                                              int currentSubCurveIndex,
                                              int* nextSubCurveIndex) const {
  assert(nextSubCurveIndex != nullptr);
  int nbOfActiveFunctions = 0;
  if (functionOrSequenceContext().displaysOnlyCartesianFunctions(
          &nbOfActiveFunctions)) {
    return FunctionGraphController::nextCurveIndexVertically(
        direction, currentCurveIndex, currentSubCurveIndex, nextSubCurveIndex);
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
  OMG::ExpiringPointer<const ContinuousFunction> function =
      functionOrSequenceContext().modelForRecord(record);
  if (function->properties().isCartesian()) {
    return FunctionGraphController::defaultCursorT(record, ignoreMargins);
  }

  if (function->properties().isScatterPlot()) {
    float t = 0;
    for (Coordinate2D<float> p : function->iterateScatterPlot()) {
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
    currentXY = function->evaluateXYAtParameter(currentT, 0);
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
  OMG::ExpiringPointer<const ContinuousFunction> f =
      functionOrSequenceContext().modelForRecord(record);
  int derivationOrder =
      f->derivationOrderFromSubCurveIndex(m_selectedSubCurveIndex);
  m_curveParameterController.setRecord(record, derivationOrder);
  /* Remove cursor highlighting and ring to ensure clean cursor if we enter
   * another controllers */
  setCursorIsRing(false);
  m_cursorView.setHighlighted(false);
  stackController()->push(&m_curveParameterController);
}

bool GraphController::moveCursorVertically(OMG::VerticalDirection direction) {
  int currentActiveFunctionIndex = *m_selectedCurveIndex;
  int nextSubCurve = 0;
  int nextCurve =
      nextCurveIndexVertically(direction, currentActiveFunctionIndex,
                               m_selectedSubCurveIndex, &nextSubCurve);
  if (nextCurve < 0) {
    return false;
  }

  OMG::ExpiringPointer<const ContinuousFunction> currentF =
      functionOrSequenceContext().modelForRecord(recordAtSelectedCurveIndex());
  float nextT =
      currentF->properties().isScatterPlot() && std::isfinite(m_cursor->x())
          ? m_cursor->x()
          : m_cursor->t();

  OMG::ExpiringPointer<const ContinuousFunction> nextF =
      functionOrSequenceContext().modelForRecord(recordAtCurveIndex(nextCurve));
  if (nextF->properties().isScatterPlot()) {
    double nextX = nextT;
    nextT = -1;
    double previousX = -INFINITY;
    for (Coordinate2D<float> xy : nextF->iterateScatterPlot()) {
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
    OMG::ExpiringPointer<const ContinuousFunction> f =
        functionOrSequenceContext().modelForRecord(currentRecord);
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
          Coordinate2D<double> xy =
              f->evaluateXYAtParameter(potentialNextT, subCurveIndex);
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
    const FunctionContext& functionContext, const Poincare::Context& context,
    bool cappedNumberOfSignificantDigits) {
  OMG::ExpiringPointer<const ContinuousFunction> function =
      GlobalContextAccessor::ContinuousFunctionContext().modelForRecord(record);
  PointsOfInterestCache* pointsOfInterest = pointsOfInterestForRecord(record);
  bannerView()->emptyInterestMessages();
  /* The interests are sorted from most important to lowest, in case there is
   * not enough space on the banner to display all of them. */
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::LocalMinimum)) {
    assert(!function->isAlongY());
    bannerView()->addInterestMessage(I18n::Message::Minimum);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::LocalMaximum)) {
    assert(!function->isAlongY());
    bannerView()->addInterestMessage(I18n::Message::Maximum);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::Intersection)) {
    bannerView()->addInterestMessage(I18n::Message::Intersection);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::Root)) {
    bannerView()->addInterestMessage(
        function->isAlongY() ? I18n::Message::LineYInterceptDescription
                             : I18n::Message::Zero);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::YIntercept)) {
    bannerView()->addInterestMessage(
        function->isAlongY() ? I18n::Message::Zero
                             : I18n::Message::LineYInterceptDescription);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(
          cursorX, cursorY, Solver<double>::Interest::UnreachedDiscontinuity)) {
    // Display undef in banner view
    cursorY = NAN;
    bannerView()->addInterestMessage(I18n::Message::Discontinuity);
  }
  bool hasInterest = bannerView()->numberOfInterestMessages() > 0;
  m_cursorView.setHighlighted(hasInterest);

  /* Cap number of significant digits for point of interest to be consistent
   * with CalculationGraphController */
  cappedNumberOfSignificantDigits =
      cappedNumberOfSignificantDigits || hasInterest;
  FunctionGraphController::reloadBannerViewForCursorOnFunction(
      cursorT, cursorX, cursorY, record, functionContext, context,
      cappedNumberOfSignificantDigits);
}

}  // namespace Graph
