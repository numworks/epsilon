#include "points_of_interest_list.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>

using namespace Poincare;
using namespace Shared;

namespace Graph {

// PointsOfInterestList::Iterator

PointsOfInterestList::Iterator & PointsOfInterestList::Iterator::operator++() {
  int n = m_list->m_list.numberOfChildren();
  do {
    ++m_index;
  } while (m_interest != Solver<double>::Interest::None && m_index < n && m_list->pointAtIndex(m_index).interest() != m_interest);
  return *this;
}

// PointsOfInterestList

Range1D PointsOfInterestList::setBoundsAndCompute(float start, float end) {
  assert(start < end);
  assert(!m_record.isNull());

  float dirtyStart = end, dirtyEnd = start;

  uint32_t checksum = m_record.checksum();
  if (m_checksum != checksum) {
    /* Discard the old results if the  record has changed. */
    m_start = m_end = NAN;
    if (!m_list.isUninitialized() && m_list.numberOfChildren() > 0) {
      dirtyStart = pointAtIndex(0).x();
      dirtyEnd = pointAtIndex(m_list.numberOfChildren() -1).x();
    }
    m_list = List::Builder();
  }

  float oldStart = m_start, oldEnd = m_end;
  m_start = start;
  m_end = end;

  if (m_list.isUninitialized()) {
    m_list = List::Builder();
  } else {
    stripOutOfBounds();
  }

  if (start > oldEnd || end < oldStart || std::isnan(oldEnd) || std::isnan(oldStart)) {
    computeBetween(start, end, &dirtyStart, &dirtyEnd);
  } else {
    if (start < oldStart) {
      computeBetween(start, oldStart, &dirtyStart, &dirtyEnd);
    }
    if (end > oldEnd) {
      computeBetween(oldEnd, end, &dirtyStart, &dirtyEnd);
    }
  }

  m_checksum = m_record.checksum();
  return Range1D(dirtyStart, dirtyEnd);
}

PointOfInterest PointsOfInterestList::pointAtIndex(int i) const {
  assert(!m_list.isUninitialized());
  assert(0 <= i && i < m_list.numberOfChildren());
  /* We need to call TreeHandle::childAtIndex instead of
   * Expression::childAtIndex, since a PointOfInterest is not an Expression. */
  TreeHandle h = static_cast<const TreeHandle &>(m_list).childAtIndex(i);
  assert(h.size() == sizeof(PointOfInterestNode));
  return static_cast<PointOfInterest &>(h);
}

void PointsOfInterestList::stripOutOfBounds() {
  assert(!m_list.isUninitialized());

  for (int i = m_list.numberOfChildren() - 1; i >= 0; i--) {
    float x = static_cast<float>(pointAtIndex(i).x());
    if (x < m_start || m_end < x) {
      m_list.removeChildAtIndexInPlace(i);
    }
  }
}

void PointsOfInterestList::computeBetween(float start, float end, float * dirtyStart, float * dirtyEnd) {
  assert(!m_list.isUninitialized());

  ContinuousFunctionStore * store = App::app()->functionStore();
  Context * context = App::app()->localContext();
  ExpiringPointer<ContinuousFunction> f = store->modelForRecord(m_record);
  Expression e = f->expressionReduced(context);

  typedef Coordinate2D<double> (Solver<double>::*NextSolution)(Expression e);
  NextSolution methodsNext[] = { &Solver<double>::nextRoot, &Solver<double>::nextMinimum, &Solver<double>::nextMaximum };
  for (NextSolution next : methodsNext) {
    Solver<double> solver = PoincareHelpers::Solver<double>(start, end, ContinuousFunction::k_unknownName, context);
    Coordinate2D<double> solution = (solver.*next)(e);
    while (std::isfinite(solution.x1())) {
      append(solution.x1(), solution.x2(), solver.lastInterest(), dirtyStart, dirtyEnd);
      solution = (solver.*next)(e);
    }
  }

  int n = store->numberOfActiveFunctions();
  for (int i = 0; i < n; i++) {
    Ion::Storage::Record record = store->activeRecordAtIndex(i);
    if (record == m_record) {
      continue;
    }
    ExpiringPointer<ContinuousFunction> g = store->modelForRecord(record);
    Expression e2 = g->expressionReduced(context);
    Solver<double> solver = PoincareHelpers::Solver<double>(start, end, ContinuousFunction::k_unknownName, context);
    Coordinate2D<double> intersection = solver.nextIntersection(e, e2);
    while (std::isfinite(intersection.x1())) {
      append(intersection.x1(), intersection.x2(), Solver<double>::Interest::Intersection, dirtyStart, dirtyEnd);
      intersection = solver.nextIntersection(e, e2);
    }
  }
}

void PointsOfInterestList::append(double x, double y, Solver<double>::Interest interest, float * dirtyStart, float * dirtyEnd) {
  assert(!m_list.isUninitialized());
  int i = 0;
  int n = m_list.numberOfChildren();
  while (i < n && pointAtIndex(i).x() < x) {
    i++;
  }
  m_list.addChildAtIndexInPlace(PointOfInterest::Builder(x, y, interest), i, n);
  *dirtyStart = std::min(*dirtyStart, static_cast<float>(x));
  *dirtyEnd = std::max(*dirtyEnd, static_cast<float>(x));
}

}
