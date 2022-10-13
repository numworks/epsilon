#include <poincare/point_of_interest.h>

namespace Poincare {

// PointOfInterest

template<typename T>
PointOfInterest<T> PointOfInterest<T>::Builder(T x, T y, typename Solver<T>::Interest interest, uint32_t data) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(PointOfInterestNode<T>));
  PointOfInterestNode<T> * node = new (bufferNode) PointOfInterestNode(x, y ,interest, data);
  TreeHandle handle = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<PointOfInterest &>(handle);
}

// PointsOfInterestList

template<typename T>
PointOfInterest<T> PointsOfInterestList<T>::pointAtIndex(int i) const {
  assert(!m_list.isUninitialized());
  assert(0 <= i && i < m_list.numberOfChildren());
  /* We need to call TreeHandle::childAtIndex instead of
   * Expression::childAtIndex, since a PointOfInterest is not an Expression. */
  TreeHandle h = static_cast<const TreeHandle &>(m_list).childAtIndex(i);
  assert(h.size() == sizeof(PointOfInterestNode<T>));
  return static_cast<PointOfInterest<T> &>(h);
}

template<typename T>
void PointsOfInterestList<T>::append(T x, T y, uint32_t data, typename Solver<T>::Interest interest) {
  assert(!m_list.isUninitialized());
  int i = 0;
  int n = m_list.numberOfChildren();
  while (i < n && pointAtIndex(i).x() < x) {
    i++;
  }
  m_list.addChildAtIndexInPlace(PointOfInterest<T>::Builder(x, y, interest, data), i, n);
}

template PointOfInterest<double> PointsOfInterestList<double>::pointAtIndex(int) const;
template void PointsOfInterestList<double>::append(double, double, uint32_t, Solver<double>::Interest);

}
