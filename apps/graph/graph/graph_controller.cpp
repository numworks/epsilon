#include "graph_controller.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/cosine.h>
#include <poincare/sine.h>
#include <poincare/matrix.h>
#include <poincare/symbol.h>
#include <algorithm>

using namespace Poincare;
using namespace Shared;
using namespace Escher;

namespace Graph {

GraphController::GraphController(Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, int * indexFunctionSelectedByCursor, ButtonRowController * header) :
  FunctionGraphController(parentResponder, inputEventHandlerDelegate, header, curveViewRange, &m_view, cursor, indexFunctionSelectedByCursor),
  m_bannerView(this, inputEventHandlerDelegate, this),
  m_view(curveViewRange, m_cursor, &m_bannerView, &m_cursorView),
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
  m_view.setInterest(Solver<double>::Interest::None);
  m_cursorView.resetMemoization();
  m_view.setCursorView(&m_cursorView);
  FunctionGraphController::viewWillAppear();
  selectFunctionWithCursor(indexFunctionSelectedByCursor(), true);
}

void GraphController::didBecomeFirstResponder() {
  FunctionGraphController::didBecomeFirstResponder();
  m_view.selectRecord(functionStore()->activeRecordAtIndex(indexFunctionSelectedByCursor()));
}

bool GraphController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Idle) {
    // Compute the points of interest when the user is not active
    m_view.resumePointsOfInterestDrawing();
    return true;
  }
  return Shared::FunctionGraphController::handleEvent(event);
}

template <typename T> static Coordinate2D<T> evaluator(T t, const void * model, Context * context) {
  const ContinuousFunction * f = static_cast<const ContinuousFunction *>(model);
  return f->evaluateXYAtParameter(t, context);
}
template <typename T> static Coordinate2D<T> evaluatorSecondCurve(T t, const void * model, Context * context) {
  const ContinuousFunction * f = static_cast<const ContinuousFunction *>(model);
  return f->evaluateXYAtParameter(t, context, 1);
}
template <typename T, int coordinate> static Coordinate2D<T> parametricExpressionEvaluator(T t, const void * model, Context * context) {
  const Expression * e = static_cast<const Expression *>(model);
  assert(e->numberOfChildren() == 2);
  assert(coordinate == 0 || coordinate == 1);
  T value = PoincareHelpers::ApproximateWithValueForSymbol<T>(e->childAtIndex(coordinate), ContinuousFunction::k_unknownName, t, context);
  return Coordinate2D<T>(t, value);
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
      Expression e = f->expressionReduced(context);
      if (f->properties().isPolar()) {
        // Turn r(theta) into f(theta) = [x(theta), y(theta)]
        Expression theta = Symbol::Builder(ContinuousFunction::k_unknownName, strlen(ContinuousFunction::k_unknownName));
        // x(theta) = cos(theta)*r(theta)
        Expression firstRow = Multiplication::Builder(Cosine::Builder(theta.clone()), e.clone());
        // y(theta) = sin(theta)*r(theta)
        Expression secondRow = Multiplication::Builder(Sine::Builder(theta.clone()), e.clone());
        Matrix parametricExpression = Matrix::Builder();
        parametricExpression.addChildAtIndexInPlace(firstRow, 0, 0);
        parametricExpression.addChildAtIndexInPlace(secondRow, 1, 1);
        parametricExpression.setDimensions(2, 1);
        e = parametricExpression;
      }
      assert((e.type() == ExpressionNode::Type::Matrix && e.numberOfChildren() == 2)
          || (e.type() == ExpressionNode::Type::Dependency && e.childAtIndex(0).type() == ExpressionNode::Type::Matrix && e.childAtIndex(0).numberOfChildren() == 2));

      // Compute the ordinate range of x(t) and y(t)
      Range1D ranges[2];
      Zoom::Function2DWithContext<float> floatEvaluators[2] = {parametricExpressionEvaluator<float, 0>, parametricExpressionEvaluator<float, 1>};
      Zoom::Function2DWithContext<double> doubleEvaluators[2] = {parametricExpressionEvaluator<double, 0>, parametricExpressionEvaluator<double, 1>};
      for (int coordinate = 0; coordinate < 2; coordinate++) {
        Zoom zoomAlongCoordinate(NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(), context, k_maxFloat);
        zoomAlongCoordinate.setBounds(f->tMin(), f->tMax());
        zoomAlongCoordinate.fitPointsOfInterest(floatEvaluators[coordinate], &e, false, doubleEvaluators[coordinate]);
        zoomAlongCoordinate.fitBounds(floatEvaluators[coordinate], &e, false);
        ranges[coordinate] = *zoomAlongCoordinate.range(false, false).y();
      }

      // Fit the zoom to the range of x(t) and y(t)
      zoom.fitPoint(Coordinate2D<float>(ranges[0].max(), ranges[1].max()));
      zoom.fitPoint(Coordinate2D<float>(ranges[0].min(), ranges[1].min()));
    } else {
      assert(f->properties().isCartesian());
      bool alongY = f->isAlongY();
      Range1D * bounds = alongY ? &yBounds : &xBounds;
      // Use the intersection between the definition domain of f and the bounds
      zoom.setBounds(
        std::clamp(f->tMin(), bounds->min(), bounds->max()),
        std::clamp(f->tMax(), bounds->min(), bounds->max())
      );
      zoom.fitPointsOfInterest(evaluator<float>, f.operator->(), alongY, evaluator<double>);
      zoom.fitBounds(evaluator<float>, f.operator->(), alongY);
      if (f->numberOfSubCurves() > 1) {
        assert(f->numberOfSubCurves() == 2);
        zoom.fitPointsOfInterest(evaluatorSecondCurve<float>, f.operator->(), alongY, evaluatorSecondCurve<double>);
        zoom.fitBounds(evaluatorSecondCurve<float>, f.operator->(), alongY);
      }

      /* Special case for piecewise functions: we want to display the branch
       * edges even if the behaviour is not "interesting".
       * FIXME For simplicity's sake, only the first piecewise operator will
       * have its conditions fitted. It is assumed that expressions containing
       * more than one piecewise will be rare. */
      Expression p;
      Expression::ExpressionTestAuxiliary yieldPiecewise = [](const Expression e, Context *, void * auxiliary) {
        if (e.type() == ExpressionNode::Type::PiecewiseOperator) {
          *static_cast<Expression *>(auxiliary) = e;
          return true;
        } else {
          return false;
        }
      };
      if (f->expressionReduced(context).recursivelyMatches(yieldPiecewise, context, ExpressionNode::SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition, &p)) {
        assert(!p.isUninitialized() && p.type() == ExpressionNode::Type::PiecewiseOperator);
        zoom.fitConditions(static_cast<PiecewiseOperator &>(p), evaluator<float>, f.operator->(), ContinuousFunction::k_unknownName, Preferences::sharedPreferences()->complexFormat(), Preferences::sharedPreferences()->angleUnit(), alongY);
      }

      /* Do not compute intersections if store is full because re-creating a
       * ContinuousFunction object each time a new function is intersected
       * is very slow. */
      if (!store->memoizationIsFull() && f->properties().canComputeIntersectionsWithFunctionsAlongSameVariable()) {
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
  assert(m_selectedSubCurveIndex < App::app()->functionStore()->modelForRecord(record)->numberOfSubCurves());
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

double GraphController::defaultCursorT(Ion::Storage::Record record, bool ignoreMargins) {
  ExpiringPointer<ContinuousFunction> function = functionStore()->modelForRecord(record);
  if (function->properties().isCartesian()) {
    return FunctionGraphController::defaultCursorT(record, ignoreMargins);
  }

  assert(function->properties().isParametric() || function->properties().isPolar());
  Poincare::Context * context = textFieldDelegateApp()->localContext();
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

void GraphController::moveCursorAndCenterIfNeeded(double t) {
  FunctionGraphController::moveCursorAndCenterIfNeeded(t);
  if (snapToInterestAndUpdateCursor(m_cursor, std::nextafter(m_cursor->t(), -static_cast<double>(INFINITY)), std::nextafter(m_cursor->t(), static_cast<double>(INFINITY)))) {
    reloadBannerView();
  }
}

void GraphController::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, FunctionStore * functionStore, Poincare::Context * context, bool cappedNumberOfSignificantDigits) {
  ExpiringPointer<ContinuousFunction> function = App::app()->functionStore()->modelForRecord(record);
  PointsOfInterestCache * pointsOfInterest = pointsOfInterestForRecord(record);
  bannerView()->emptyInterestMessages(&m_cursorView);
  /* The interests are sorted from most important to lowest, in case there is
   * not enough space on the banner to display all of them. */
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(cursor->x(), cursor->y(), Solver<double>::Interest::LocalMinimum)) {
    assert(!function->isAlongY());
    bannerView()->addInterestMessage(I18n::Message::Minimum, &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(cursor->x(), cursor->y(), Solver<double>::Interest::LocalMaximum)) {
    assert(!function->isAlongY());
    bannerView()->addInterestMessage(I18n::Message::Maximum, &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(cursor->x(), cursor->y(), Solver<double>::Interest::Intersection)) {
    bannerView()->addInterestMessage(I18n::Message::Intersection, &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(cursor->x(), cursor->y(), Solver<double>::Interest::Root)) {
    bannerView()->addInterestMessage(function->isAlongY() ? I18n::Message::LineYInterceptDescription : I18n::Message::Zero, &m_cursorView);
  }
  if (pointsOfInterest->hasDisplayableInterestAtCoordinates(cursor->x(), cursor->y(), Solver<double>::Interest::YIntercept)) {
    bannerView()->addInterestMessage(function->isAlongY() ? I18n::Message::Zero : I18n::Message::LineYInterceptDescription, &m_cursorView);
  }

  /* Cap number of significant digits for point of interest to be consistent
   * with CalculationGraphController */
  cappedNumberOfSignificantDigits = cappedNumberOfSignificantDigits || bannerView()->numberOfInterestMessages() > 0;
  FunctionGraphController::reloadBannerViewForCursorOnFunction(cursor, record, functionStore, context, cappedNumberOfSignificantDigits);
}

}
