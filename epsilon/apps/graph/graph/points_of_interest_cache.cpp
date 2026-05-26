#include "points_of_interest_cache.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/numeric_solver/intersection_solver.h>
#include <poincare/numeric_solver/solver.h>

#include <algorithm>
#include <array>

#include "../app.h"
#include "shared/global_store.h"

using namespace Poincare;
using namespace Poincare::NumericSolver;
using namespace Shared;

namespace Graph {

// PointsOfInterestCache

void PointsOfInterestCache::setBounds(double start, double end) {
  assert(start <= end);

  uint32_t checksum = Ion::Storage::FileSystem::sharedFileSystem->checksum();
  if (m_checksum != checksum) {
    /* Discard the old results if anything in the storage has changed. */
    m_computedStart = m_computedEnd = start;
    m_list.init();
    m_interestingPointsOverflowPool = false;
  }

  m_start = start;
  m_end = end;
  m_computedEnd = std::clamp(m_computedEnd, start, end);
  m_computedStart = std::clamp(m_computedStart, start, end);

  if (m_list.isUninitialized()) {
    m_list.init();
    m_interestingPointsOverflowPool = false;
  } else {
    stripOutOfBounds();
  }

  m_checksum = checksum;
}

bool PointsOfInterestCache::computeUntilNthPoint(int n) {
  while (n >= numberOfPoints() && !isFullyComputed()) {
    if (!computeNextStep(true)) {
      return false;
    }
  }
  return true;
}

bool PointsOfInterestCache::PointFitInterest(PointOfInterest poi,
                                             Interest interest) {
  return interest == Interest::None || poi.interest == interest;
}

int PointsOfInterestCache::numberOfPoints(Interest interest) const {
  int n = numberOfPoints();
  if (interest == Poincare::Interest::None) {
    return n;
  }
  int result = 0;
  for (int i = 0; i < n; i++) {
    if (PointFitInterest(pointAtIndex(i), interest)) {
      result++;
    }
  }
  return result;
}

PointOfInterest PointsOfInterestCache::firstPointInDirection(
    double xStart, double xEnd, double yStart, bool stretch, Interest interest,
    int subCurveIndex, bool alongX) {
  if (xStart == xEnd) {
    return PointOfInterest();
  }
  // yStart parameter is only expected when finding points along X.
  assert(std::isnan(yStart) || alongX);
  m_list.sort(alongX);
  int n = numberOfPoints();
  int direction = xStart > xEnd ? -1 : 1;
  int firstIndex = 0;
  int lastIndex = n - 1;
  if (direction < 0) {
    std::swap(firstIndex, lastIndex);
  }
  for (int i = firstIndex; direction * i <= direction * lastIndex;
       i += direction) {
    PointOfInterest p = pointAtIndex(i);
    double x = alongX ? p.x() : p.abscissa;
    if (direction * x >= direction * xEnd) {
      break;
    }
    if (direction * x < direction * xStart) {
      continue;
    }
    /* Use RoughlyEqual because interest point can be too close. This can happen
     * when an intersection is detected twice at an almost identical abscissa.
     */
    if (OMG::Float::RoughlyEqual<double>(x, xStart, 1e-8)) {
      double y = alongX ? p.y() : p.ordinate;
      if (std::isnan(yStart) || direction * y < direction * yStart ||
          OMG::Float::RoughlyEqual<double>(y, yStart, 1e-8)) {
        continue;
      }
    }
    if (!PointFitInterest(p, interest) ||
        (subCurveIndex >= 0 && p.subCurveIndex != subCurveIndex)) {
      continue;
    }
    if (p.interest == Interest::UnreachedDiscontinuity &&
        direction * (i + direction) <= direction * lastIndex) {
      /* Select in priority the reached discontinuity point: if the point is an
       * unreached discontinuity, check if there is a reached discontinuity at
       * the same abscissa. */
      PointOfInterest pNext = pointAtIndex(i + direction);
      double xNext = alongX ? pNext.x() : pNext.abscissa;
      if (pNext.interest == Interest::ReachedDiscontinuity && xNext == x) {
        return pNext;
      }
    }
    return p;
  }
  return PointOfInterest{};
}

PointOfInterest PointsOfInterestCache::pointOfInterestAtCoordinates(
    double x, double y, Interest interest) const {
  int n = numberOfPoints();
  for (int i = 0; i < n; i++) {
    PointOfInterest p = pointAtIndex(i);
    if (p.x() == x && p.y() == y && PointFitInterest(p, interest)) {
      return p;
    }
  }
  return PointOfInterest{};
}

PointOfInterest PointsOfInterestCache::displayablePointOfInterestAtCoordinates(
    double x, double y, Poincare::Interest interest) const {
  if (!canDisplayPoints(interest)) {
    // Ignore interest point if it is not displayed.
    return Poincare::PointOfInterest{};
  }
  return pointOfInterestAtCoordinates(x, y, interest);
}

double PointsOfInterestCache::cacheStep() const {
  /* If the bounds are large enough, there might be less than k_numberOfSteps
   * doubles between them. */
  double result = (m_end - m_start) / k_numberOfSteps;
  double minimalStep = std::max(std::fabs(m_end), std::fabs(m_start)) *
                       OMG::Float::Epsilon<double>();
  return std::max(result, minimalStep);
}

double PointsOfInterestCache::solverStep() const {
  return std::fabs(m_end - m_start) / 100.0;
}

void PointsOfInterestCache::stripOutOfBounds() {
  assert(!m_list.isUninitialized());
  m_list.filterOutOfBounds(m_start, m_end);
}

bool PointsOfInterestCache::computeNextStep(bool allowUserInterruptions) {
  Expression newPoints;
  {
    CircuitBreakerCheckpoint cbcp(Ion::CircuitBreaker::CheckpointType::AnyKey);
    if (!allowUserInterruptions || CircuitBreakerRun(cbcp)) {
      double nextStart = m_computedEnd;
      double nextEnd = m_computedStart;
      double step = cacheStep();
      if (m_computedEnd < m_end) {
        nextEnd = std::clamp(m_computedEnd + step, m_start, m_end);
        if (nextEnd + solverStep() >= m_end) {
          /* If there is less than one solver step remaining, just compute until
           * the end. This avoids having a very small cache step at the end
           * because of accumulated numerical errors, which could cause issues
           * in the solver. */
          nextEnd = m_end;
        }
      } else if (m_computedStart > m_start) {
        nextStart = std::clamp(m_computedStart - step, m_start, m_end);
        if (nextStart - solverStep() <= m_start) {
          // See comment above
          nextStart = m_start;
        }
      } else {
        assert(false);
      }
      newPoints = computeBetween(nextStart, nextEnd);
    } else {
      tidyDownstreamPoolFrom(cbcp.endOfPoolBeforeCheckpoint());
      return false;
    }
  }
  if (newPoints.isUninitialized() || !m_list.merge(newPoints)) {
    m_interestingPointsOverflowPool = true;
    return false;
  }
  return true;
}

namespace {

struct PointSearchContext {
  const double start, end;
  const double searchStep;
  const SymbolContext& context;
  ContinuousFunctionStore* const store;
  Ion::Storage::Record record;

  RootSolver rootSolver;
  ExtremumSolver extremumSolver;
  DiscontinuitySolver discontinuitySolver;

  size_t currentProvider = 0;
  int counter = 0;
  struct {
    uint8_t subCounter1 = 0;
    uint8_t subCounter2 = 0;
  };
  /* TODO: Intersections handling is a bit messy because we need to recreate a
   * new solver for each pair of functions intersected. */
  double currentStartForIntersections;
  Ion::Storage::Record otherRecord;

  bool lastSegment;

  OMG::ExpiringPointer<ContinuousFunction> model() const {
    return store->modelForRecord(record);
  }

  void resetCurrentStartForIntersections() {
    currentStartForIntersections = start;
  }
};

/* TODO: When alongY, this interest point is later converted to a Root. This
 * could be done earlier to limit risks of miss-interpretation.*/
PointOfInterest findYIntercept(PointSearchContext* ctx) {
  if (ctx->start >= 0.f || 0.f >= ctx->end) {
    return PointOfInterest{};
  }
  OMG::ExpiringPointer<ContinuousFunction> f = ctx->model();
  int n = f->numberOfSubCurves();
  while (ctx->counter < n) {
    uint8_t subCurve = ctx->counter++;
    Coordinate2D<double> xy = f->evaluateXYAtParameter(0., subCurve);
    if (std::isfinite(xy.x()) && std::isfinite(xy.y())) {
      if (f->isAlongY()) {
        // evaluateXYAtParameter has swapped x and y. Revert it.
        xy = Coordinate2D<double>(xy.y(), xy.x());
      }
      return {.abscissa = xy.x(),
              .ordinate = xy.y(),
              .interest = Interest::YIntercept,
              .inverted = f->isAlongY(),
              .isUnreached = f->properties().isStrictInequality(),
              .subCurveIndex = static_cast<uint8_t>(subCurve)};
    }
  }
  return PointOfInterest{};
}

/* TODO: When alongY, this interest point is later converted to a YIntercept.
 * This could be done earlier to limit risks of miss-interpretation.*/
PointOfInterest findRootOrExtremum(PointSearchContext* ctx) {
  OMG::ExpiringPointer<ContinuousFunction> f = ctx->model();
  if (f->numberOfSubCurves() != 1) {
    // Only y-intercept
    return PointOfInterest{};
  }

  NumericSolver::Solver<double>* solvers[] = {
      &ctx->rootSolver, &ctx->extremumSolver, &ctx->discontinuitySolver};
  for (; ctx->counter < static_cast<int>(std::size(solvers)); ctx->counter++) {
    NumericSolver::Solver<double>* currentSolver = solvers[ctx->counter];
    if (currentSolver != &ctx->rootSolver && f->isAlongY()) {
      // Do not compute min and max since they would appear left/rightmost
      continue;
    }
    Solution<double> solution;
    while (std::isfinite((solution = currentSolver->next()).x)) {
      /* Loop over finite solutions to exhaust solutions out of the interval
       * without returning NAN. */
      if (solution.x >= ctx->start &&
          (solution.x < ctx->end ||
           (solution.x == ctx->end && ctx->lastSegment))) {
        /* If alongY(), no need to swap x and y here, since
         * expressionApproximated already returned x as abscissa and y as
         * ordinate. */
        return {
            .abscissa = solution.x,
            .ordinate = solution.y,
            .interest = solution.interest,
            .inverted = f->isAlongY(),
            .isUnreached =
                f->properties().isStrictInequality() ||
                solution.interest == Interest::UnreachedDiscontinuity,
        };
      }
    }
  }
  return PointOfInterest{};
}

PointOfInterest findIntersectionsAux(PointSearchContext* ctx,
                                     const PreparedFunction& f,
                                     const PreparedFunction& g,
                                     bool alongSameAxis) {
  Solution<double> solution;

  IntersectionSolver solver(ctx->currentStartForIntersections, ctx->end, f, g,
                            alongSameAxis);
  solver.setSearchStep(ctx->searchStep);
  solver.setGrowthSpeed(GrowthSpeed::Precise);
  // Only stretch the end, or would stay stuck at the first solution.
  solver.stretch(true);

  while (std::isfinite((solution = solver.next()).x)) {
    /* Loop over finite solutions to exhaust solutions out of the interval
     * without returning NAN. */
    if (solution.x >= ctx->start &&
        (solution.x < ctx->end ||
         (solution.x == ctx->end && ctx->lastSegment))) {
      assert(solution.interest == Interest::Intersection);
      ctx->currentStartForIntersections = solution.x;
      /* If alongY(), no need to swap x and y here, since
       * expressionApproximated already returned x as abscissa and y as
       * ordinate. */
      return {
          .abscissa = solution.x,
          .ordinate = solution.y,
          .data = static_cast<uint32_t>(ctx->otherRecord),
          .interest = Interest::Intersection,
          .inverted = ctx->model()->isAlongY(),
          .subCurveIndex = ctx->subCounter1,
      };
    }
  }
  ctx->resetCurrentStartForIntersections();
  return PointOfInterest{};
}

PointOfInterest findIntersections(PointSearchContext* ctx) {
  /* Do not compute intersections if store is full because re-creating a
   * ContinuousFunction object each time a new function is intersected
   * is very slow. */
  OMG::ExpiringPointer<ContinuousFunction> f = ctx->model();
  if ((ctx->store->memoizationOverflows() ||
       !f->shouldDisplayIntersections())) {
    return PointOfInterest{};
  }
  // shouldDisplayIntersections might invalidate cache, refetch f
  f = ctx->model();
  int n = ctx->store->numberOfModels();
  const PreparedFunction& fExpr = f->expressionApproximated();
  bool fAlongY = f->isAlongY();
  bool fIsStrict = f->properties().isStrictInequality();
  int fNumberOfSubCurves = fExpr.isList() ? fExpr.numberOfChildren() : 1;
  assert(!fExpr.isList() || fNumberOfSubCurves > 1);
  for (; ctx->counter < n; ++ctx->counter) {
    ctx->otherRecord = ctx->store->recordAtIndex(ctx->counter);
    if (ctx->record == ctx->otherRecord) {
      continue;
    }
    OMG::ExpiringPointer<ContinuousFunction> g =
        ctx->store->modelForRecord(ctx->otherRecord);
    bool gIsStrict = g->properties().isStrictInequality();
    bool gAlongY = g->properties().isAlongY();
    if (!g->shouldDisplayIntersections()) {
      continue;
    }
    // shouldDisplayIntersections might invalidate cache, refetch g
    g = ctx->store->modelForRecord(ctx->otherRecord);
    const PreparedFunction& gExpr = g->expressionApproximated();
    int gNumberOfSubCurves = gExpr.isList() ? gExpr.numberOfChildren() : 1;
    assert(!gExpr.isList() || gNumberOfSubCurves > 1);
    for (; ctx->subCounter1 < fNumberOfSubCurves; ++ctx->subCounter1) {
      PreparedFunction firstFunc =
          fNumberOfSubCurves > 1 ? fExpr.cloneChildAtIndex(ctx->subCounter1)
                                 : fExpr;
      for (; ctx->subCounter2 < gNumberOfSubCurves; ++ctx->subCounter2) {
        PreparedFunction secondFunc =
            gNumberOfSubCurves > 1 ? gExpr.cloneChildAtIndex(ctx->subCounter2)
                                   : gExpr;
        PointOfInterest poi = findIntersectionsAux(ctx, firstFunc, secondFunc,
                                                   fAlongY == gAlongY);
        if (!poi.isInitialized()) {
          continue;
        }
        poi.isUnreached = fIsStrict || gIsStrict;
        return poi;
      }
      ctx->subCounter2 = 0;
    }
    ctx->subCounter1 = 0;
  }

  return PointOfInterest{};
}

PointOfInterest findPoints(void* searchContext) {
  PointSearchContext* ctx = static_cast<PointSearchContext*>(searchContext);

  constexpr PointOfInterest (*providers[])(PointSearchContext*) = {
      findYIntercept, findRootOrExtremum, findIntersections};

  for (; ctx->currentProvider < std::size(providers); ctx->currentProvider++) {
    PointOfInterest p = providers[ctx->currentProvider](ctx);
    if (p.isInitialized()) {
      return p;
    }
    ctx->counter = 0;
  }

  return PointOfInterest{};
}

}  // namespace

Expression PointsOfInterestCache::computeBetween(double start, double end) {
  assert(!m_record.isNull());
  assert(m_checksum == Ion::Storage::FileSystem::sharedFileSystem->checksum());
  assert(!m_list.isUninitialized());
  assert((end == m_computedStart && start < m_computedStart) ||
         (start == m_computedEnd && end > m_computedEnd));
  assert(start >= m_start && end <= m_end);

  if (start < m_computedStart) {
    m_computedStart = start;
  } else if (end > m_computedEnd) {
    m_computedEnd = end;
  }

  ContinuousFunctionStore* store = &App::app()->functionStore();

  OMG::ExpiringPointer<ContinuousFunction> f = store->modelForRecord(m_record);
  PreparedFunction prepared = f->expressionApproximated();
  RootSolver rootSolver(start, end, prepared);
  ExtremumSolver extremumSolver(start, end, prepared);
  DiscontinuitySolver discontinuitySolver(start, end, prepared);

  PointSearchContext searchContext{
      .start = start,
      .end = end,
      .searchStep = solverStep(),
      .context = App::app()->localContext(),
      .store = store,
      .record = m_record,
      .rootSolver = rootSolver,
      .extremumSolver = extremumSolver,
      .discontinuitySolver = discontinuitySolver,
      .lastSegment = (m_computedEnd == end),
  };

  NumericSolver::Solver<double>* solvers[] = {
      &searchContext.rootSolver, &searchContext.extremumSolver,
      &searchContext.discontinuitySolver};

  for (size_t i = 0; i < std::size(solvers); i++) {
    solvers[i]->setGrowthSpeed(GrowthSpeed::Fast);
    solvers[i]->setSearchStep(searchContext.searchStep);
    solvers[i]->stretch();
  }

  searchContext.resetCurrentStartForIntersections();

  return PointsOfInterestList::BuildStash(findPoints, &searchContext);
}

void PointsOfInterestCache::tidyDownstreamPoolFrom(
    const PoolObject* treePoolCursor) const {
  ContinuousFunctionStore* functionStore =
      &GlobalContextAccessor::ContinuousFunctionStore();
  int n = functionStore->numberOfActiveFunctions();
  for (int i = 0; i < n; i++) {
    functionStore->modelForRecord(functionStore->activeRecordAtIndex(i))
        ->tidyDownstreamPoolFrom(treePoolCursor);
  }
  GlobalContextAccessor::SequenceStore().tidyDownstreamPoolFrom(treePoolCursor);
}

}  // namespace Graph
