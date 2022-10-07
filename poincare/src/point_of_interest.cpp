#include <poincare/point_of_interest.h>

namespace Poincare {

// PointOfInterest

PointOfInterest PointOfInterest::Builder(double x, double y, Solver<double>::Interest interest, uint32_t data) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(PointOfInterestNode));
  PointOfInterestNode * node = new (bufferNode) PointOfInterestNode(x, y ,interest, data);
  TreeHandle handle = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<PointOfInterest &>(handle);
}

}
