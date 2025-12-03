#include "points_of_interest_cache.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>

#include <algorithm>
#include <array>

#include "../app.h"
#include "shared/global_store.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

// PointsOfInterestCache

void PointsOfInterestCache::setBounds(float start, float end) {
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

bool PointsOfInterestCache::PointFitInterest(
    PointOfInterest poi, Solver<double>::Interest interest) {
  return interest == Solver<double>::Interest::None ||
         poi.interest == interest ||
         (interest == Solver<double>::Interest::Intersection &&
          poi.interest == Solver<double>::Interest::UnreachedIntersection);
}

int PointsOfInterestCache::numberOfPoints(
    Solver<double>::Interest interest) const {
  int n = numberOfPoints();
  if (interest == Poincare::Solver<double>::Interest::None) {
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
    double xStart, double xEnd, double yStart, bool stretch,
    Solver<double>::Interest interest, int subCurveIndex, bool alongX) {
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
    if (p.interest == Solver<double>::Interest::UnreachedDiscontinuity &&
        direction * (i + direction) <= direction * lastIndex) {
      /* Select in priority the reached discontinuity point: if the point is an
       * unreached discontinuity, check if there is a reached discontinuity at
       * the same abscissa. */
      PointOfInterest pNext = pointAtIndex(i + direction);
      double xNext = alongX ? pNext.x() : pNext.abscissa;
      if (pNext.interest == Solver<double>::Interest::ReachedDiscontinuity &&
          xNext == x) {
        return pNext;
      }
    }
    return p;
  }
  return PointOfInterest{};
}

bool PointsOfInterestCache::hasInterestAtCoordinates(
    double x, double y, Solver<double>::Interest interest) const {
  int n = numberOfPoints();
  for (int i = 0; i < n; i++) {
    PointOfInterest p = pointAtIndex(i);
    if (p.x() == x && p.y() == y && PointFitInterest(p, interest)) {
      return true;
    }
  }
  return false;
}

bool PointsOfInterestCache::hasDisplayableInterestAtCoordinates(
    double x, double y, Poincare::Solver<double>::Interest interest,
    bool allInterestsAreDisplayed) const {
  if (!canDisplayPoints(allInterestsAreDisplayed
                            ? Poincare::Solver<double>::Interest::None
                            : interest)) {
    // Ignore interest point if it is not displayed.
    return false;
  }
  return hasInterestAtCoordinates(x, y, interest);
}

bool PointsOfInterestCache::hasDisplayableUnreachedInterestAtCoordinates(
    double x, double y) const {
  using Interest = Poincare::Solver<double>::Interest;
  for (Interest interest :
       {Interest::UnreachedDiscontinuity, Interest::UnreachedIntersection}) {
    if (canDisplayPoints(interest) &&
        hasInterestAtCoordinates(x, y, interest)) {
      return true;
    }
  }
  return false;
}

float PointsOfInterestCache::step() const {
  /* If the bounds are large enough, there might be less than k_numberOfSteps
   * floats between them. */
  float result = (m_end - m_start) / k_numberOfSteps;
  float minimalStep = std::max(std::fabs(m_end), std::fabs(m_start)) *
                      OMG::Float::Epsilon<float>();
  return std::max(result, minimalStep);
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
      if (m_computedEnd < m_end) {
        newPoints = computeBetween(
            m_computedEnd, std::clamp(m_computedEnd + step(), m_start, m_end));
      } else if (m_computedStart > m_start) {
        newPoints =
            computeBetween(std::clamp(m_computedStart - step(), m_start, m_end),
                           m_computedStart);
      }
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
  const float start, end;
  const SymbolContext& context;
  ContinuousFunctionStore* const store;
  const float searchStep;
  Solver<double> solver;
  Ion::Storage::Record record;

  size_t currentProvider = 0;
  int counter = 0;
  struct {
    uint8_t subCounter1 = 0;
    uint8_t subCounter2 = 0;
  };
  Ion::Storage::Record otherRecord;

  bool lastSegment;

  void reinitSolver() { solver.reset(start, end, searchStep); }

  OMG::ExpiringPointer<ContinuousFunction> model() const {
    return store->modelForRecord(record);
  }
};

/* TODO: When alongY, this interest point is later converted to a Root. This
 * could be done earlier to limit risks of miss-interpretation.*/
PointOfInterest findYIntercept(void* searchContext) {
  PointSearchContext* ctx = static_cast<PointSearchContext*>(searchContext);
  if (ctx->start < 0.f && 0.f < ctx->end) {
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
        return {xy.x(),
                xy.y(),
                0,
                Solver<double>::Interest::YIntercept,
                f->isAlongY(),
                static_cast<uint8_t>(subCurve)};
      }
    }
  }
  return PointOfInterest{};
}

/* TODO: When alongY, this interest point is later converted to a YIntercept.
 * This could be done earlier to limit risks of miss-interpretation.*/
PointOfInterest findRootOrExtremum(void* searchContext) {
  PointSearchContext* ctx = static_cast<PointSearchContext*>(searchContext);

  OMG::ExpiringPointer<ContinuousFunction> f = ctx->model();
  if (f->numberOfSubCurves() != 1) {
    // Only y-intercept
    return PointOfInterest{};
  }

  using NextSolution =
      Solver<double>::Solution (Solver<double>::*)(const Internal::Tree*);
  NextSolution methodsNext[] = {
      &Solver<double>::nextRoot, &Solver<double>::nextMinimum,
      &Solver<double>::nextMaximum, &Solver<double>::nextDiscontinuity};
  while (ctx->counter < static_cast<int>(std::size(methodsNext))) {
    NextSolution next = methodsNext[ctx->counter];
    if (next != static_cast<NextSolution>(&Solver<double>::nextRoot) &&
        f->isAlongY()) {
      // Do not compute min and max since they would appear left/rightmost
      ++ctx->counter;
      continue;
    }
    ctx->solver.setGrowthSpeed(Solver<double>::GrowthSpeed::Fast);
    Solver<double>::Solution solution;
    while (std::isfinite(
        (solution = (ctx->solver.*next)(
             f->expressionApproximated()) /* assignment in expression */)
            .x())) {
      /* Loop over finite solutions to exhaust solutions out of the interval
       * without returning NAN. */
      if (solution.xy().xIsIn(ctx->start, ctx->end, true, ctx->lastSegment)) {
        /* If alongY(), no need to swap x and y here, since
         * expressionApproximated already returned x as abscissa and y as
         * ordinate. */
        return {
            solution.x(),        solution.y(),  0,
            solution.interest(), f->isAlongY(), 0,
        };
      }
    }
    ++ctx->counter;
    ctx->reinitSolver();
  }
  return PointOfInterest{};
}

PointOfInterest findIntersectionsAux(PointSearchContext* ctx,
                                     PreparedFunction f, PreparedFunction g,
                                     bool alongSameAxis) {
  Solver<double>::Solution solution;
  Internal::Tree* diff = nullptr;
  while (std::isfinite(
      (solution = alongSameAxis
                      ? ctx->solver.nextIntersection(
                            f, g, const_cast<const Internal::Tree**>(&diff))
                      : ctx->solver.nextIntersectionAlongDifferentAxis(
                            f, g, const_cast<const Internal::Tree**>(&diff)))
          .x())) {
    /* Loop over finite solutions to exhaust solutions out of the interval
     * without returning NAN. */
    if (solution.xy().xIsIn(ctx->start, ctx->end, true, false)) {
      assert(solution.interest() == Solver<double>::Interest::Intersection);
      diff->removeTree();
      /* If alongY(), no need to swap x and y here, since
       * expressionApproximated already returned x as abscissa and y as
       * ordinate. */
      return {
          solution.x(),
          solution.y(),
          static_cast<uint32_t>(ctx->otherRecord),
          Solver<double>::Interest::Intersection,
          ctx->model()->isAlongY(),
          ctx->subCounter1,
      };
    }
  }
  diff->removeTree();
  ctx->reinitSolver();
  return PointOfInterest{};
}

PointOfInterest findIntersections(void* searchContext) {
  PointSearchContext* ctx = static_cast<PointSearchContext*>(searchContext);
  /* Do not compute intersections if store is full because re-creating a
   * ContinuousFunction object each time a new function is intersected
   * is very slow. */
  OMG::ExpiringPointer<ContinuousFunction> f = ctx->model();
  if ((ctx->store->memoizationOverflows() ||
       !f->shouldDisplayIntersections())) {
    return PointOfInterest{};
  }
  int n = ctx->store->numberOfModels();
  PreparedFunction fExpr = f->expressionApproximated();
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
    PreparedFunction gExpr = g->expressionApproximated();
    ctx->solver.setGrowthSpeed(Solver<double>::GrowthSpeed::Precise);
    int gNumberOfSubCurves = gExpr.isList() ? gExpr.numberOfChildren() : 1;
    assert(!gExpr.isList() || gNumberOfSubCurves > 1);
    for (; ctx->subCounter1 < fNumberOfSubCurves; ++ctx->subCounter1) {
      PreparedFunction firstFunc = fExpr;
      if (fNumberOfSubCurves > 1) {
        firstFunc = fExpr.cloneChildAtIndex(ctx->subCounter1);
      }
      for (; ctx->subCounter2 < gNumberOfSubCurves; ++ctx->subCounter2) {
        PreparedFunction secondFunc = gExpr;
        if (gNumberOfSubCurves > 1) {
          secondFunc = gExpr.cloneChildAtIndex(ctx->subCounter2);
        }
        PointOfInterest poi = findIntersectionsAux(ctx, firstFunc, secondFunc,
                                                   fAlongY == gAlongY);
        if (poi.isUninitialized()) {
          continue;
        }
        if (fIsStrict || gIsStrict) {
          poi.interest = Solver<double>::Interest::UnreachedIntersection;
        }
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

  constexpr PointsOfInterestList::Provider providers[] = {
      findYIntercept, findRootOrExtremum, findIntersections};

  for (; ctx->currentProvider < std::size(providers); ctx->currentProvider++) {
    PointOfInterest p = providers[ctx->currentProvider](ctx);
    if (!p.isUninitialized()) {
      return p;
    }
    ctx->counter = 0;
  }

  return PointOfInterest{};
}

}  // namespace

Expression PointsOfInterestCache::computeBetween(float start, float end) {
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

  PointSearchContext searchContext{
      .start = start,
      .end = end,
      .context = App::app()->localContext(),
      .store = store,
      .searchStep = static_cast<float>(
          Solver<double>::DefaultSearchStepForAmplitude(m_start - m_end)),
      .solver = Solver<double>(start, end, App::app()->localContext()),
      .record = m_record,
      .lastSegment = (m_computedEnd == end),
  };
  searchContext.reinitSolver();

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
