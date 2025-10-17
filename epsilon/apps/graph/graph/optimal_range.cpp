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
  Zoom zoom(NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
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
    Coordinate2D<float> intervalMin = Coordinate2D<float>(NAN, NAN);
    Coordinate2D<float> intervalMax = intervalMin;
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
      for (int coordinate = 0; coordinate < 2; coordinate++) {
        Zoom zoomAlongCoordinate(
            NAN, NAN, InteractiveCurveViewRange::NormalYXRatio(), k_maxFloat);
        zoomAlongCoordinate.setBounds(f->tMin(), f->tMax());
        zoomAlongCoordinate.fitPointsOfInterest(floatEvaluators[coordinate], &e,
                                                false, false,
                                                doubleEvaluators[coordinate]);
        zoomAlongCoordinate.fitBounds(floatEvaluators[coordinate], &e, false);
        ranges[coordinate] = *zoomAlongCoordinate.range(false, false).y();
      }

      // Fit the zoom to the range of x(t) and y(t)
      zoom.fitPoint(Coordinate2D<float>(ranges[0].max(), ranges[1].max()));
      zoom.fitPoint(Coordinate2D<float>(ranges[0].min(), ranges[1].min()));

      // Since function is defined on an interval, add interval edges
      intervalMin = Coordinate2D<float>(
          parametricExpressionEvaluator<float, 0>(f->tMin(), &e).y(),
          parametricExpressionEvaluator<float, 1>(f->tMin(), &e).y());
      intervalMax = Coordinate2D<float>(
          parametricExpressionEvaluator<float, 0>(f->tMax(), &e).y(),
          parametricExpressionEvaluator<float, 1>(f->tMax(), &e).y());
    } else if (f->properties().isScatterPlot()) {
      onlyLines = false;
      for (Coordinate2D<float> p : f->iterateScatterPlot()) {
        zoom.fitPoint(p);
      }
    } else {
      assert(f->properties().isCartesian());
      bool isLine = f->properties().isLine();
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
      Range1D<float>* bounds = alongY ? &yBounds : &xBounds;
      // Use the intersection between the definition domain of f and the bounds
      float tMin = std::clamp(f->tMin(), bounds->min(), bounds->max());
      float tMax = std::clamp(f->tMax(), bounds->min(), bounds->max());
      zoom.setBounds(tMin, tMax);
      // If function is defined on an interval, add interval edges
      if (tMin == f->tMin()) {
        Coordinate2D<float> c(evaluator<float>(tMin, &fModel));
        intervalMin = Coordinate2D<float>(tMin, alongY ? c.x() : c.y());
      }
      if (tMax == f->tMax()) {
        Coordinate2D<float> c(evaluator<float>(tMax, &fModel));
        intervalMax = Coordinate2D<float>(tMax, alongY ? c.x() : c.y());
      }

      zoom.fitPointsOfInterest(evaluator<float>, &fModel, alongY,
                               isLine && !alongY, evaluator<double>,
                               canComputeIntersections + i);
      zoom.fitBounds(evaluator<float>, &fModel, alongY);
      if (f->numberOfSubCurves() > 1) {
        assert(f->numberOfSubCurves() == 2);
        zoom.fitPointsOfInterest(
            evaluatorSecondCurve<float>, &fModel, alongY, isLine && !alongY,
            evaluatorSecondCurve<double>, canComputeIntersections + i);
        zoom.fitBounds(evaluatorSecondCurve<float>, &fModel, alongY);
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
                           evaluator<float>, &fModel, alongY);
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
                                  &gModel);
          }
        }
      }
    }
    if (std::isfinite(intervalMin.x()) && std::isfinite(intervalMin.y())) {
      zoom.fitPoint(intervalMin);
    }
    if (std::isfinite(intervalMax.x()) && std::isfinite(intervalMax.y())) {
      zoom.fitPoint(intervalMax);
    }
  }
  if (onlyLines && !onlyLinearLines) {
    // Zoom out on non-linear lines to show y-intercept better
    zoom.zoom(Zoom<float>::k_lineZoomOutRatio);
  }

  if (computeY) {
    zoom.setBounds(xBounds.min(), xBounds.max());
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
    // Only one of the ranges has been increased
    assert(xRangeRatio >= 1.f && yRangeRatio >= 1.f);
    // Only one of the ranges has been altered
    assert(xRangeRatio == 1.f || yRangeRatio == 1.f);

    float rangeRatio = xRangeRatio + yRangeRatio - 1.0f;
    /* Normalization is also enforced in Zoom::range up to
     * Zoom::k_maxNormalizationRatio. rangeRatio is therefore either 1
     * (normalization did not need to be forced) or bigger. */
    assert(rangeRatio >= 1.0f);
    constexpr float k_maximalRatio = 5.f;
    static_assert(k_maximalRatio > Zoom<float>::k_maxNormalizationRatio,
                  "k_maximalRatio is too small to ever be reached.");
    constexpr float k_maxRelativeRatio =
        k_maximalRatio / Zoom<float>::k_maxNormalizationRatio;
    if (rangeRatio > k_maxRelativeRatio) {
      newRange = unNormalizedRange;
    }
  }

  return Range2D<float>(*(computeX ? newRange : originalRange).x(),
                        *(computeY ? newRange : originalRange).y());
}
}  // namespace Graph
