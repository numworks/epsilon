#include <poincare/point_of_interest.h>

namespace Poincare {

// PointOfInterest

PointOfInterest PointOfInterest::Builder(double abscissa, double ordinate, typename Solver<double>::Interest interest, uint32_t data, bool inverted, int subCurveIndex) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(PointOfInterestNode));
  PointOfInterestNode * node = new (bufferNode) PointOfInterestNode(abscissa, ordinate ,interest, data, inverted, subCurveIndex);
  TreeHandle handle = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<PointOfInterest &>(handle);
}

// PointsOfInterestList

PointOfInterest PointsOfInterestList::pointAtIndex(int i) const {
  assert(!m_list.isUninitialized());
  assert(0 <= i && i < m_list.numberOfChildren());
  /* We need to call TreeHandle::childAtIndex instead of
   * Expression::childAtIndex, since a PointOfInterest is not an Expression. */
  TreeHandle h = static_cast<const TreeHandle &>(m_list).childAtIndex(i);
  assert(h.size() == sizeof(PointOfInterestNode));
  return static_cast<PointOfInterest &>(h);
}

void PointsOfInterestList::append(double abscissa, double ordinate, uint32_t data, typename Solver<double>::Interest interest, bool inverted, int subCurveIndex) {
  assert(!m_list.isUninitialized());
  int n = m_list.numberOfChildren();
  if (interest == Solver<double>::Interest::Root) {
    // Sometimes the root is close to zero but not exactly zero
    ordinate = 0.0;
  }
  m_list.addChildAtIndexInPlace(PointOfInterest::Builder(abscissa, ordinate, interest, data, inverted, subCurveIndex), n, n);
}

void PointsOfInterestList::sort() {
  Helpers::Sort(
    [](int i, int j, void * context, int numberOfElements) {
      List list =  static_cast<PointsOfInterestList *>(context)->list();
      list.swapChildrenInPlace(i, j);
    },
    [](int i, int j, void * context, int numberOfElements) {
      PointsOfInterestList * pointsList = static_cast<PointsOfInterestList *>(context);
      return pointsList->pointAtIndex(i).abscissa() > pointsList->pointAtIndex(j).abscissa();
    },
    (void *)this,
    numberOfPoints()
  );
}

}
