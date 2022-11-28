#include "points_of_interest_cache.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>

using namespace Poincare;
using namespace Shared;

namespace Graph {

// PointsOfInterestCache

PointsOfInterestCache PointsOfInterestCache::clone() const {
  PointsOfInterestCache result = *this;
  Expression cloneList = result.list().clone();
  result.m_list.setList(static_cast<List&>(cloneList));
  return result;
}

Range1D PointsOfInterestCache::setBounds(float start, float end) {
  assert(start < end);
  assert(!m_record.isNull());

  uint32_t checksum = Ion::Storage::FileSystem::sharedFileSystem()->checksum();
  if (m_checksum != checksum || m_computedRecord != m_record) {
    /* Discard the old results if anything in the storage has changed. */
    m_computedStart = m_computedEnd = start;
    m_list.init();
  }

  m_start = start;
  m_end = end;
  m_computedEnd = std::clamp(m_computedEnd, start, end);
  m_computedStart = std::clamp(m_computedStart, start, end);

  if (m_list.isUninitialized()) {
    m_list.init();
  } else {
    stripOutOfBounds();
  }

  m_checksum = checksum;
  m_computedRecord = m_record;
  return Range1D(0.0, 1.0);
}

Poincare::PointOfInterest PointsOfInterestCache::computePointAtIndex(int i) {
  while (i >= m_list.numberOfPoints() && m_computedEnd < m_end) {
    computeBetween(m_computedEnd, std::clamp(m_computedEnd + step(), m_start, m_end));
  }
  while (i >= m_list.numberOfPoints() && m_computedStart > m_start) {
    computeBetween(std::clamp(m_computedStart - step(), m_start, m_end), m_computedStart);
  }
  return i < m_list.numberOfPoints() ? m_list.pointAtIndex(i) : PointOfInterest();
}

PointOfInterest PointsOfInterestCache::firstPointInDirection(double start, double end, Solver<double>::Interest interest) {
  assert(start != end);
  m_list.sort();
  int n = m_list.numberOfPoints();
  int direction = start > end ? -1 : 1;
  int firstIndex = 0;
  int lastIndex = n - 1;
  if (direction < 0) {
    std::swap(firstIndex, lastIndex);
  }
  for (int i = firstIndex; direction * i <= direction * lastIndex; i += direction) {
    PointOfInterest p = m_list.pointAtIndex(i);
    if (direction * p.x() <= direction * start) {
      continue;
    }
    if (direction * p.x() >= direction * end) {
      break;
    }
    if (interest == Solver<double>::Interest::None || interest == p.interest()) {
      return p;
    }
  }
  return PointOfInterest();
}

PointOfInterest PointsOfInterestCache::pointOfInterestAtAbscissa(double x) {
  m_list.sort();
  int n = m_list.numberOfPoints();
  for (int i = 0; i < n; i++) {
    PointOfInterest p = m_list.pointAtIndex(i);
    if (p.x() == x) {
      return p;
    }
    if (p.x() > x) {
      break;
    }
  }
  return PointOfInterest();
}


void PointsOfInterestCache::stripOutOfBounds() {
  assert(!m_list.isUninitialized());

  for (int i = m_list.numberOfPoints() - 1; i >= 0; i--) {
    float x = static_cast<float>(m_list.pointAtIndex(i).x());
    if (x < m_start || m_end < x) {
      m_list.list().removeChildAtIndexInPlace(i);
    }
  }
}

void PointsOfInterestCache::computeBetween(float start, float end) {
  assert(!m_list.isUninitialized());

  // TODO: Check that start and end are not completely out of [m_computedStart, m_computedEnd]
  // assert(end == m_computedEnd || start == m_computedStart);
  if (start < m_computedStart && end == m_computedStart) {
    m_computedStart = start;
  }
  if (start == m_computedEnd && end > m_computedEnd) {
    m_computedEnd = end;
  }

  ContinuousFunctionStore * store = App::app()->functionStore();
  Context * context = App::app()->localContext();
  ExpiringPointer<ContinuousFunction> f = store->modelForRecord(m_record);
  Expression e = f->expressionReduced(context);

  if (!f->isAlongY() && start < 0.f && 0.f < end) {
    Coordinate2D<double> xy = f->evaluateXYAtParameter(0., context);
    if (std::isfinite(xy.x1()) && std::isfinite(xy.x2())) {
      append(xy.x1(), xy.x2(), Solver<double>::Interest::YIntercept);
    }
  }

  typedef Coordinate2D<double> (Solver<double>::*NextSolution)(const Expression & e);
  NextSolution methodsNext[] = { &Solver<double>::nextRoot, &Solver<double>::nextMinimum, &Solver<double>::nextMaximum };
  for (NextSolution next : methodsNext) {
    Solver<double> solver = PoincareHelpers::Solver<double>(start, end, ContinuousFunction::k_unknownName, context);
    solver.stretch();
    Coordinate2D<double> solution;
    while (std::isfinite((solution = (solver.*next)(e)).x1())) { // assignment in condition
      append(solution.x1(), solution.x2(), solver.lastInterest());
    }
  }

  if (!f->shouldDisplayIntersections()) {
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
    Solver<double> solver = PoincareHelpers::Solver<double>(start, end, ContinuousFunction::k_unknownName, context);
    solver.stretch();
    Expression diff;
    Coordinate2D<double> intersection;
    while (std::isfinite((intersection = solver.nextIntersection(e, e2, &diff)).x1())) { // assignment in condition
      assert(sizeof(record) == sizeof(uint32_t));
      append(intersection.x1(), intersection.x2(), Solver<double>::Interest::Intersection,  *reinterpret_cast<uint32_t *>(&record));
    }
  }

}

void PointsOfInterestCache::append(double x, double y, Solver<double>::Interest interest,uint32_t data) {
  assert(std::isfinite(x) && std::isfinite(y));
  m_list.append(x, y, data, interest);
}

}
