#include "optimal_range.h"

#include <apps/shared/continuous_function.h>
#include <apps/shared/continuous_function_store.h>
#include <apps/shared/function.h>
#include <apps/shared/interactive_curve_view_range.h>
#include <omg/expiring_pointer.h>
#include <poincare/coordinate_2D.h>
#include <poincare/numeric_solver/zoom.h>
#include <poincare/prepared_function.h>
#include <poincare/range.h>
#include <poincare/src/memory/tree.h>

using namespace Poincare;
using namespace Shared;

namespace Graph {

struct ContinuousFunctionAndContext {
  const ContinuousFunction* func;
  const SymbolContext* ctx;
};

template <typename T>
static Coordinate2D<T> evaluator(T t, const void* model) {
  const ContinuousFunctionAndContext curve =
      *static_cast<const ContinuousFunctionAndContext*>(model);
  assert(curve.ctx);
  return curve.func->evaluateXYAtParameter(t);
}
template <typename T>
static Coordinate2D<T> evaluatorSecondCurve(T t, const void* model) {
  const ContinuousFunctionAndContext curve =
      *static_cast<const ContinuousFunctionAndContext*>(model);
  assert(curve.ctx);
  return curve.func->evaluateXYAtParameter(t, 1);
}

template <typename T, int coordinate>
static Coordinate2D<T> parametricExpressionEvaluator(T t, const void* model) {
  const PreparedFunctionPoint* e =
      static_cast<const PreparedFunctionPoint*>(model);
  assert(e->isPoint());
  assert(coordinate == 0 || coordinate == 1);
  // TODO: Approximating the other coordinate could be skipped for performances.
  Coordinate2D<T> value =
      e->approximateToPointOrRealScalarWithValue<T>(t).toPoint();
  return Coordinate2D<T>(t, (coordinate == 0) ? value.x() : value.y());
}

Range2D<float> OptimalRange(bool computeX, bool computeY,
                            Range2D<float> originalRange,
                            const ContinuousFunctionStore* store,
                            const bool enforceNormalizedRange,
                            const SymbolContext& symbolContext) {
  constexpr float k_maxFloat = InteractiveCurveViewRange::k_maxFloat;
  Zoom zoom(InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
  if (store->memoizationOverflows()) {
    /* Do not compute autozoom if store is full because the computation is too
     * slow. */
    Range1D<float> xRange =
        Range1D<float>::FromHalfLength(Range1D<float>::k_defaultHalfLength);
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
  bool tryNormalizedRange = false;

  bool onlyLines = true;
  bool onlyLinearLines = true;
  for (int i = 0; i < nbFunctions; i++) {
    canComputeIntersections[i] = false;
    OMG::ExpiringPointer<const ContinuousFunction> f =
        store->modelForRecord(store->activeRecordAtIndex(i));
    ContinuousFunctionAndContext fModel{.func = f.operator->(),
                                        .ctx = &symbolContext};
    tryNormalizedRange |= f->properties().enforcePlotNormalization();
    if (f->approximationBasedOnCostlyAlgorithms()) {
      onlyLines = false;
      continue;
    }
    if (f->properties().isPolar() || f->properties().isInversePolar() ||
        f->properties().isParametric()) {
      assert(std::isfinite(f->tMin()) && std::isfinite(f->tMax()));
      onlyLines = false;
      PreparedFunctionPoint e = f->parametricForm().getPreparedFunction(
          Shared::Function::k_unknownName);
      // Compute the ordinate range of x(t) and y(t)
      Range1D<float> ranges[2];
      Function2D<float> floatEvaluators[2] = {
          parametricExpressionEvaluator<float, 0>,
          parametricExpressionEvaluator<float, 1>};
      Function2D<double> doubleEvaluators[2] = {
          parametricExpressionEvaluator<double, 0>,
          parametricExpressionEvaluator<double, 1>};
      Range1D<float> searchBounds(f->tMin(), f->tMax());
      for (int coordinate = 0; coordinate < 2; coordinate++) {
        Zoom zoomAlongCoordinate(InteractiveCurveViewRange::NormalYXRatio(),
                                 k_maxFloat);

        // Fit bounds
        zoomAlongCoordinate.fitFunctionAtEdges(
            searchBounds, floatEvaluators[coordinate], &e, false);

        // Fit the points of interest of x(t) or y(t)
        zoomAlongCoordinate.fitPointsOfInterest(floatEvaluators[coordinate], &e,
                                                searchBounds, false, false,
                                                doubleEvaluators[coordinate]);

        // Fit magnitude on bounds
        zoomAlongCoordinate.fitMagnitude(floatEvaluators[coordinate], &e, true);

        ranges[coordinate] = *zoomAlongCoordinate.range(false, false).y();
      }

      // Fit the zoom to the range of x(t) and y(t)
      zoom.fitPoint(Coordinate2D<float>(ranges[0].max(), ranges[1].max()));
      zoom.fitPoint(Coordinate2D<float>(ranges[0].min(), ranges[1].min()));

    } else if (f->properties().isScatterPlot()) {
      onlyLines = false;
      for (Coordinate2D<float> p : f->iterateScatterPlot()) {
        zoom.fitPoint(p);
      }
    } else {
      assert(f->properties().isCartesian());
      bool isLine = f->properties().isCartesianLine();
      if (isLine) {
        // Affine function is linear if f(0) = 0
        onlyLinearLines =
            onlyLinearLines && std::fabs(evaluator<float>(0, &fModel).y()) <=
                                   OMG::Float::Epsilon<float>();
      } else {
        onlyLines = false;
      }
      canComputeIntersections[i] = true;
      bool alongY = f->isAlongY();

      zoom.fitFunctionAtEdges(Range1D<float>(f->tMin(), f->tMax()),
                              evaluator<float>, &fModel, alongY);

      Range1D<float>* bounds = alongY ? &yBounds : &xBounds;
      // Use the intersection between the definition domain of f and the bounds
      float tMin = std::clamp(f->tMin(), bounds->min(), bounds->max());
      float tMax = std::clamp(f->tMax(), bounds->min(), bounds->max());
      Range1D<float> searchBounds(tMin, tMax);

      zoom.fitPointsOfInterest(evaluator<float>, &fModel, searchBounds, alongY,
                               isLine && !alongY, evaluator<double>,
                               canComputeIntersections + i);

      if (f->numberOfSubCurves() > 1) {
        assert(f->numberOfSubCurves() == 2);
        zoom.fitFunctionAtEdges(Range1D<float>(f->tMin(), f->tMax()),
                                evaluatorSecondCurve<float>, &fModel, alongY);
        zoom.fitPointsOfInterest(evaluatorSecondCurve<float>, &fModel,
                                 searchBounds, alongY, isLine && !alongY,
                                 evaluatorSecondCurve<double>,
                                 canComputeIntersections + i);
      }

      /* Special case for piecewise functions: we want to display the branch
       * edges even if the behaviour is not "interesting".
       * FIXME For simplicity's sake, only the first piecewise operator will
       * have its conditions fitted. It is assumed that expressions containing
       * more than one piecewise will be rare. */
      const Internal::Tree* piecewise =
          f->expressionApproximated().tree()->firstDescendantSatisfying(
              [](const Internal::Tree* t) { return t->isPiecewise(); });
      if (piecewise) {
        zoom.fitConditions(PreparedFunction::Builder(piecewise),
                           evaluator<float>, &fModel, searchBounds, alongY);
      }

      if (canComputeIntersections[i] &&
          f->properties()
              .canComputeIntersectionsWithFunctionsAlongSameVariable()) {
        for (int j = 0; j < i; j++) {
          OMG::ExpiringPointer<const ContinuousFunction> g =
              store->modelForRecord(store->activeRecordAtIndex(j));
          if (canComputeIntersections[j] &&
              g->properties()
                  .canComputeIntersectionsWithFunctionsAlongSameVariable() &&
              g->isAlongY() == alongY &&
              !g->approximationBasedOnCostlyAlgorithms()) {
            ContinuousFunctionAndContext gModel{.func = g.operator->(),
                                                .ctx = &symbolContext};
            zoom.fitIntersections(evaluator<float>, &fModel, evaluator<float>,
                                  &gModel, searchBounds);
          }
        }
      }
    }
  }
  if (onlyLines && !onlyLinearLines && zoom.hasInterestingRange()) {
    /* NOTE: zoom.hasInterestingRange() could be asserted if line functions
     * always produced a valid interesting range.
     * Currently, this is not the case. For instance "y=b" with "b" an undefined
     * variable is considered to be a Line. This "line" however is undefined
     * everywhere and this results in the "interesting range" to be NAN. */
    // Zoom out on non-linear lines to show y-intercept better
    zoom.zoom(Zoom<float>::k_lineZoomOutRatio);
  }

  if (computeY) {
    for (int i = 0; i < nbFunctions; i++) {
      OMG::ExpiringPointer<const ContinuousFunction> f =
          store->modelForRecord(store->activeRecordAtIndex(i));
      if (f->approximationBasedOnCostlyAlgorithms() ||
          !f->properties().isCartesian()) {
        continue;
      }
      bool alongY = f->isAlongY();
      /* If X range is forced (computeX is false), we don't want to crop the Y
       * axis: we want to see the value of f at each point of X range. */
      bool cropOutliers = computeX;
      ContinuousFunctionAndContext model{.func = f.operator->(),
                                         .ctx = &symbolContext};
      zoom.fitMagnitude(evaluator, &model, cropOutliers, alongY);
      if (f->numberOfSubCurves() > 1) {
        zoom.fitMagnitude(evaluatorSecondCurve, &model, cropOutliers, alongY);
      }
    }
  }

  Range2D<float> newRange =
      zoom.range(true, enforceNormalizedRange || tryNormalizedRange);

  if (!enforceNormalizedRange && tryNormalizedRange) {
    // Cancel normalization if the normalized range is too extended
    Range2D<float> unNormalizedRange = zoom.range(true, false);
    float xRangeRatio =
        newRange.x()->length() / unNormalizedRange.x()->length();
    float yRangeRatio =
        newRange.y()->length() / unNormalizedRange.y()->length();

    /* Normalization is also partially enforced in Zoom::range up to a variable
     * max ratio. We want to avoid applying more than double this
     * pseudo-normalization. */
    constexpr float k_maxRangeRatio = 2.f;
    if (std::max(xRangeRatio, yRangeRatio) > k_maxRangeRatio) {
      newRange = unNormalizedRange;
    }
  }

  return Range2D<float>(*(computeX ? newRange : originalRange).x(),
                        *(computeY ? newRange : originalRange).y());
}
}  // namespace Graph
