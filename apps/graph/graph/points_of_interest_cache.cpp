#include "points_of_interest_cache.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/circuit_breaker_checkpoint.h>
#include <poincare/exception_checkpoint.h>

#include <algorithm>

#include "../app.h"

using namespace Poincare;
using namespace Shared;

namespace Graph {

// PointsOfInterestCache

PointsOfInterestCache PointsOfInterestCache::clone() const {
  PointsOfInterestCache result = *this;
  Expression cloneList = result.list().clone();
  result.m_list.setList(static_cast<List &>(cloneList));
  return result;
}

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
    Poincare::Solver<double>::Interest interest) const {
  int n = numberOfPoints();
  if (interest == Poincare::Solver<double>::Interest::None) {
    return n;
  }
  int result = 0;
  for (int i = 0; i < n; i++) {
    if (pointAtIndex(i).interest() == interest) {
      result++;
    }
  }
  return result;
}

PointOfInterest PointsOfInterestCache::firstPointInDirection(
    double start, double end, Solver<double>::Interest interest,
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
    if (direction * p.abscissa() <= direction * start) {
      continue;
    }
    if (direction * p.abscissa() >= direction * end) {
      break;
    }
    if ((interest == Solver<double>::Interest::None ||
         interest == p.interest()) &&
        p.subCurveIndex() == subCurveIndex) {
      return p;
    }
  }
  return PointOfInterest();
}

bool PointsOfInterestCache::hasInterestAtCoordinates(
    double x, double y, Solver<double>::Interest interest) const {
  int n = numberOfPoints();
  for (int i = 0; i < n; i++) {
    PointOfInterest p = pointAtIndex(i);
    if (p.x() == x && p.y() == y &&
        (interest == Solver<double>::Interest::None ||
         p.interest() == interest)) {
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
  float minimalStep =
      std::max(std::fabs(m_end), std::fabs(m_start)) * Float<float>::Epsilon();
  return std::max(result, minimalStep);
}

void PointsOfInterestCache::stripOutOfBounds() {
  assert(!m_list.isUninitialized());

  int initialNumberOfPoints = numberOfPoints();
  for (int i = initialNumberOfPoints - 1; i >= 0; i--) {
    float x = static_cast<float>(pointAtIndex(i).abscissa());
    if (x < m_start || m_end < x) {
      m_list.list().removeChildAtIndexInPlace(i);
      m_interestingPointsOverflowPool = false;
    }
  }
}

bool PointsOfInterestCache::computeNextStep(bool allowUserInterruptions) {
  // Clone the cache to prevent modifying the pool before the checkpoint
  PointsOfInterestCache cacheClone;
  {
    /* Always use an ExceptionCheckpoint in case cloning or computing interest
     * points overflows the pool. */
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      /* If allowed, use a CircuitBreakerCheckpoint so that computation can be
       * interrupted to allow plot navigation in parallel of computation. */
      CircuitBreakerCheckpoint checkpoint(
          Ion::CircuitBreaker::CheckpointType::AnyKey);
      if (!allowUserInterruptions || CircuitBreakerRun(checkpoint)) {
        cacheClone = clone();
        if (m_computedEnd < m_end) {
          cacheClone.computeBetween(
              m_computedEnd,
              std::clamp(m_computedEnd + step(), m_start, m_end));
        } else if (m_computedStart > m_start) {
          cacheClone.computeBetween(
              std::clamp(m_computedStart - step(), m_start, m_end),
              m_computedStart);
        }
      } else {
        tidy();
        return false;
      }
    } else {
      // TODO : Notify the user that the pool is full
      m_interestingPointsOverflowPool = true;
      tidy();
      return false;
    }
  }
  *this = cacheClone;
  return true;
}

void PointsOfInterestCache::computeBetween(float start, float end) {
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

  float searchStep = Solver<double>::MaximalStep(m_start - m_end);

  ContinuousFunctionStore *store = App::app()->functionStore();
  Context *context = App::app()->localContext();
  ExpiringPointer<ContinuousFunction> f = store->modelForRecord(m_record);
  Expression e = f->expressionReduced(context);

  if (start < 0.f && 0.f < end) {
    for (int curveIndex = 0; curveIndex < f->numberOfSubCurves();
         curveIndex++) {
      Coordinate2D<double> xy =
          f->evaluateXYAtParameter(0., context, curveIndex);
      if (std::isfinite(xy.x()) && std::isfinite(xy.y())) {
        if (f->isAlongY()) {
          xy = Coordinate2D<double>(xy.y(), xy.x());
        }
        append(xy.x(), xy.y(), Solver<double>::Interest::YIntercept, 0,
               curveIndex);
      }
    }
  }

  typedef Coordinate2D<double> (Solver<double>::*NextSolution)(
      const Expression &e);
  NextSolution methodsNext[] = {&Solver<double>::nextRoot,
                                &Solver<double>::nextMinimum,
                                &Solver<double>::nextMaximum};
  for (NextSolution next : methodsNext) {
    if (next != static_cast<NextSolution>(&Solver<double>::nextRoot) &&
        f->isAlongY()) {
      // Do not compute min and max since they would appear left/rightmost
      continue;
    }
    Solver<double> solver = PoincareHelpers::Solver<double>(
        start, end, ContinuousFunction::k_unknownName, context);
    solver.setSearchStep(searchStep);
    solver.stretch();
    Coordinate2D<double> solution;
    while (std::isfinite(
        (solution = (solver.*next)(e)).x())) {  // assignment in condition
      /* Ensure that the solution is in [start, end), even if the interval was
       * stretched. */
      if (!solution.xIsIn(start, end, true, false)) {
        continue;
      }
      append(solution.x(), solution.y(), solver.lastInterest());
    }
  }

  /* Do not compute intersections if store is full because re-creating a
   * ContinuousFunction object each time a new function is intersected
   * is very slow. */
  if (store->memoizationIsFull() || !f->shouldDisplayIntersections()) {
    return;
  }

  int n = store->numberOfActiveFunctions();
  for (int i = 0; i < n; i++) {
    Ion::Storage::Record record = store->activeRecordAtIndex(i);
    if (record == m_record) {
      continue;
    }
    ExpiringPointer<ContinuousFunction> g = store->modelForRecord(record);
    if (!g->shouldDisplayIntersections()) {
      continue;
    }
    Expression e2 = g->expressionReduced(context);
    Solver<double> solver = PoincareHelpers::Solver<double>(
        start, end, ContinuousFunction::k_unknownName, context);
    solver.setSearchStep(searchStep);
    solver.stretch();
    Expression diff;
    Coordinate2D<double> intersection;
    while (std::isfinite((intersection = solver.nextIntersection(e, e2, &diff))
                             .x())) {  // assignment in condition
      assert(sizeof(record) == sizeof(uint32_t));
      /* Ensure that the intersection is in [start, end), even if the interval
       * was stretched. */
      if (!intersection.xIsIn(start, end, true, false)) {
        continue;
      }
      append(intersection.x(), intersection.y(),
             Solver<double>::Interest::Intersection,
             *reinterpret_cast<uint32_t *>(&record));
    }
  }
}

void PointsOfInterestCache::append(double x, double y,
                                   Solver<double>::Interest interest,
                                   uint32_t data, int subCurveIndex) {
  assert(std::isfinite(x) && std::isfinite(y));
#if __EMSCRIPTEN__
  // Cap the total number of points
  if (m_interestingPointsOverflowPool ||
      (numberOfPoints() > k_numberOfPointsToOverflowEmscripten)) {
    m_interestingPointsOverflowPool = true;
    return;
  }
#endif
  ExpiringPointer<ContinuousFunction> f =
      App::app()->functionStore()->modelForRecord(m_record);
  m_list.append(x, y, data, interest, f->isAlongY(), subCurveIndex);
}

void PointsOfInterestCache::tidy() const {
  ContinuousFunctionStore *store = App::app()->functionStore();
  int n = store->numberOfActiveFunctions();
  for (int i = 0; i < n; i++) {
    store->modelForRecord(store->activeRecordAtIndex(i))
        ->tidyDownstreamPoolFrom();
  }
  App::app()->localContext()->tidyDownstreamPoolFrom();
}

}  // namespace Graph
