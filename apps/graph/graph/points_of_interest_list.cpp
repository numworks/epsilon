#include "points_of_interest_list.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>

using namespace Poincare;
using namespace Shared;

namespace Graph {

void PointsOfInterestList::setBoundsAndCompute(float start, float end) {
  assert(start < end);
  assert(!m_record.isNull());

  float oldStart = m_start, oldEnd = m_end;
  m_start = start;
  m_end = end;

  if (m_list.isUninitialized()) {
    m_list = List::Builder();
  } else {
    stripOutOfBounds();
  }

  if (start > oldEnd || end < oldStart || std::isnan(oldEnd) || std::isnan(oldStart)) {
    computeBetween(start, end);
  } else {
    if (start < oldStart) {
      computeBetween(start, oldStart);
    }
    if (end > oldEnd) {
      computeBetween(oldEnd, end);
    }
  }

  m_checksum = m_record.checksum();
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

void PointsOfInterestList::computeBetween(float start, float end) {
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
      append(solution.x1(), solution.x2(), solver.lastInterest());
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
      append(intersection.x1(), intersection.x2(), Solver<double>::Interest::Intersection);
      intersection = solver.nextIntersection(e, e2);
    }
  }
}

void PointsOfInterestList::append(double x, double y, Solver<double>::Interest interest) {
  assert(!m_list.isUninitialized());
  int i = 0;
  int n = m_list.numberOfChildren();
  while (i < n && pointAtIndex(i).x() < x) {
    i++;
  }
  m_list.addChildAtIndexInPlace(PointOfInterest::Builder(x, y, interest), i, n);
}

}
