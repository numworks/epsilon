#include <poincare/point_of_interest.h>

namespace Poincare {

// PointOfInterest

PointOfInterest PointOfInterest::Builder(double x, double y, Solver<double>::Interest interest) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(PointOfInterestNode));
  PointOfInterestNode * node = new (bufferNode) PointOfInterestNode(x, y ,interest);
  TreeHandle handle = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<PointOfInterest &>(handle);
}

}
