#include <poincare/point_of_interest.h>

namespace Poincare {

// PointOfInterest

PointOfInterest PointOfInterest::Builder(double x, double y, typename Solver<double>::Interest interest, uint32_t data) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(PointOfInterestNode));
  PointOfInterestNode * node = new (bufferNode) PointOfInterestNode(x, y ,interest, data);
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

void PointsOfInterestList::append(double x, double y, uint32_t data, typename Solver<double>::Interest interest) {
  assert(!m_list.isUninitialized());
  int n = m_list.numberOfChildren();
  /*while (i < n && pointAtIndex(i).x() < x) {
    i++;
  }*/
  m_list.addChildAtIndexInPlace(PointOfInterest::Builder(x, y, interest, data), n, n);
}

}
