#include "points_of_interest_cache.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/old/circuit_breaker_checkpoint.h>
#include <poincare/old/exception_checkpoint.h>

#include <algorithm>
#include <array>

#include "../app.h"

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

int PointsOfInterestCache::numberOfPoints(
    Solver<double>::Interest interest) const {
  int n = numberOfPoints();
  if (interest == Poincare::Solver<double>::Interest::None) {
    return n;
  }
  int result = 0;
  for (int i = 0; i < n; i++) {
    if (pointAtIndex(i).interest == interest) {
      result++;
    }
  }
  return result;
}

PointOfInterest PointsOfInterestCache::firstPointInDirection(
    double start, double end, bool stretch, Solver<double>::Interest interest,
    int subCurveIndex) {
  if (start == end) {
    return PointOfInterest();
  }
  m_list.sort();
  int n = numberOfPoints();
  int direction = start > end ? -1 : 1;
  int firstIndex = 0;
  int lastIndex = n - 1;
  if (direction < 0) {
    std::swap(firstIndex, lastIndex);
  }
  for (int i = firstIndex; direction * i <= direction * lastIndex;
       i += direction) {
    PointOfInterest p = pointAtIndex(i);
    if (direction * p.abscissa < direction * start ||
        (!stretch && p.abscissa == start)) {
      continue;
    }
    if (direction * p.abscissa > direction * end ||
        (!stretch && p.abscissa == end)) {
      break;
    }
    if ((interest == Solver<double>::Interest::None ||
         interest == p.interest) &&
        p.subCurveIndex == subCurveIndex) {
      /* Select in priority the reached discontinuity point: if the point is an
       * unreached discontinuity, check if there is a reached discontinuity at
       * the same abscissa. */
      if (p.interest == Solver<double>::Interest::UnreachedDiscontinuity &&
          direction * (i + direction) <= direction * lastIndex) {
        PointOfInterest nextP = pointAtIndex(i + direction);
        if (nextP.interest == Solver<double>::Interest::ReachedDiscontinuity &&
            nextP.x() == p.x()) {
          return nextP;
        }
      }
      return p;
    }
  }
  return PointOfInterest{};
}

bool PointsOfInterestCache::hasInterestAtCoordinates(
    double x, double y, Solver<double>::Interest interest) const {
  int n = numberOfPoints();
  for (int i = 0; i < n; i++) {
    PointOfInterest p = pointAtIndex(i);
    if (p.x() == x && p.y() == y &&
        (interest == Solver<double>::Interest::None ||
         p.interest == interest)) {
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
  return PointsOfInterestCache::hasInterestAtCoordinates(x, y, interest);
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
  API::JuniorPoolHandle newPoints;
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
  Context* const context;
  ContinuousFunctionStore* const store;
  const float searchStep;
  Solver<double> solver;
  Ion::Storage::Record record;

  size_t currentProvider = 0;
  int counter = 0;
  Ion::Storage::Record otherRecord;
  SystemFunction memoizedOtherFunction;

  void reinitSolver() {
    solver = {start, end, context};
    solver.setSearchStep(searchStep);
    solver.stretch();
  }
  ExpiringPointer<ContinuousFunction> model() const {
    return store->modelForRecord(record);
  }
};

PointOfInterest findYIntercept(void* searchContext) {
  PointSearchContext* ctx = static_cast<PointSearchContext*>(searchContext);
  if (ctx->start < 0.f && 0.f < ctx->end) {
    ExpiringPointer<ContinuousFunction> f = ctx->model();
    int n = f->numberOfSubCurves();
    while (ctx->counter < n) {
      uint8_t subCurve = ctx->counter++;
      Coordinate2D<double> xy =
          f->evaluateXYAtParameter(0., ctx->context, subCurve);
      if (std::isfinite(xy.x()) && std::isfinite(xy.y())) {
        if (f->isAlongY()) {
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

PointOfInterest findRootOrExtremum(void* searchContext) {
  PointSearchContext* ctx = static_cast<PointSearchContext*>(searchContext);

  ExpiringPointer<ContinuousFunction> f = ctx->model();
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
    while (
        std::isfinite((solution = (ctx->solver.*next)(f->expressionApproximated(
                           ctx->context)) /* assignment in expression */)
                          .x())) {
      /* Loop over finite solutions to exhaust solutions out of the interval
       * without returning NAN. */
      if (solution.xy().xIsIn(ctx->start, ctx->end, true, false)) {
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

PointOfInterest findIntersections(void* searchContext) {
  PointSearchContext* ctx = static_cast<PointSearchContext*>(searchContext);
  /* Do not compute intersections if store is full because re-creating a
   * ContinuousFunction object each time a new function is intersected
   * is very slow. */
  ExpiringPointer<ContinuousFunction> f = ctx->model();
  if ((ctx->store->memoizationOverflows() ||
       !f->shouldDisplayIntersections())) {
    return PointOfInterest{};
  }
  int n = ctx->store->numberOfActiveFunctions();
  SystemFunction e = f->expressionApproximated(ctx->context);
  bool alongY = f->isAlongY();
  while (ctx->counter < n) {
    int otherFunctionIndex = ctx->counter;
    if (ctx->memoizedOtherFunction.isUninitialized()) {
      ctx->otherRecord = ctx->store->recordAtIndex(otherFunctionIndex);
      if (ctx->record == ctx->otherRecord) {
        ++ctx->counter;
        continue;
      }
      ExpiringPointer<ContinuousFunction> g =
          ctx->store->modelForRecord(ctx->otherRecord);
      if (!g->shouldDisplayIntersections()) {
        ++ctx->counter;
        continue;
      }
      ctx->memoizedOtherFunction = g->expressionApproximated(ctx->context);
    }
    ctx->solver.setGrowthSpeed(Solver<double>::GrowthSpeed::Precise);
    Solver<double>::Solution solution;
    while (std::isfinite(
        (solution = ctx->solver.nextIntersection(e, ctx->memoizedOtherFunction))
            .x())) {
      /* Loop over finite solutions to exhaust solutions out of the interval
       * without returning NAN. */
      if (solution.xy().xIsIn(ctx->start, ctx->end, true, false)) {
        return {
            solution.x(),
            solution.y(),
            static_cast<uint32_t>(ctx->otherRecord),
            solution.interest(),
            alongY,
            0,
        };
      }
    }
    ++ctx->counter;
    ctx->memoizedOtherFunction = SystemFunction{};
    ctx->reinitSolver();
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

API::JuniorPoolHandle PointsOfInterestCache::computeBetween(float start,
                                                            float end) {
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

  ContinuousFunctionStore* store = App::app()->functionStore();
  Context* context = App::app()->localContext();

  PointSearchContext searchContext{
      .start = start,
      .end = end,
      .context = context,
      .store = store,
      .searchStep = static_cast<float>(
          Solver<double>::DefaultSearchStepForAmplitude(m_start - m_end)),
      .solver = {start, end, context},
      .record = m_record,
  };
  searchContext.reinitSolver();

  return PointsOfInterestList::BuildStash(findPoints, &searchContext);
}

void PointsOfInterestCache::tidyDownstreamPoolFrom(
    PoolObject* treePoolCursor) const {
  ContinuousFunctionStore* store = App::app()->functionStore();
  int n = store->numberOfActiveFunctions();
  for (int i = 0; i < n; i++) {
    store->modelForRecord(store->activeRecordAtIndex(i))
        ->tidyDownstreamPoolFrom(treePoolCursor);
  }
  GlobalContext::s_sequenceStore->tidyDownstreamPoolFrom(treePoolCursor);
}

}  // namespace Graph
