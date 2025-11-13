#include "intersection_region_graph_controller.h"

#include "../app.h"

using namespace Poincare;

namespace Graph {

// Return true if x is a closer than bestX in the given direction
bool IsBetterPoint(bool directionIsRight, double x, double bestX) {
  return (directionIsRight && x < bestX) || (!directionIsRight && x > bestX);
}

void IntersectionRegionGraphController::updateBestPointOfInterestForRecord(
    Ion::Storage::Record record, double start, double max, bool stretch,
    bool directionIsRight, PointOfInterestData& bestPoint) {
  OMG::ExpiringPointer<Shared::ContinuousFunction> f =
      functionStore()->modelForRecord(record);
  if (f->properties().isEquality()) {
    // Only display intersections with inequalities
    return;
  }
  bool isAlongY = f->properties().isAlongY();
  while (true) {
    PointOfInterest p = computeAtLeastOnePointOfInterest(
        App::app()->graphController()->pointsOfInterestForRecord(record), start,
        max, stretch);
    if (p.isUninitialized()) {
      // No more points of interest for this record
      return;
    }
    // Point of interest can be out of range with functions along Y.
    if (!isAlongY || (IsBetterPoint(directionIsRight, start, p.x()) &&
                      IsBetterPoint(directionIsRight, p.x(), max))) {
      assert(IsBetterPoint(directionIsRight, start, p.x()) &&
             IsBetterPoint(directionIsRight, p.x(), max));
      // Point must be better in x, or in y with functions along Y at same x.
      if (IsBetterPoint(directionIsRight, p.x(), bestPoint.coordinate.x()) ||
          (isAlongY && p.x() == bestPoint.coordinate.x() &&
           IsBetterPoint(directionIsRight, p.y(), bestPoint.coordinate.y()))) {
        Ion::Storage::Record intersectedRecord = Ion::Storage::Record(p.data);
        // Only display intersections with inequalities
        if (!functionStore()
                 ->modelForRecord(intersectedRecord)
                 ->properties()
                 .isEquality()) {
          // Update best point of interest
          bestPoint.coordinate = p.xy();
          bestPoint.isUnreachedIntersection =
              (p.interest == Solver<double>::Interest::UnreachedIntersection);
          bestPoint.record = record;
          bestPoint.intersectedRecord = intersectedRecord;
          if (!isAlongY) {
            // No need to explore further
            return;
          }
        }
      }
    }
    if (start == p.x() && !stretch) {
      // This should only happen with functions along Y.
      assert(isAlongY);
      // computeAtLeastOnePointOfInterest would return the same point again.
      /* TODO: computeAtLeastOnePointOfInterest doesn't handle well functions
       * along Y. If multiple points of interest are at the same x coordinate,
       * only one can be returned. */
      return;
    }
    // Try another point of interest
    start = p.x();
    // Prevent finding the same point again
    stretch = false;
  }
  OMG::unreachable();
}

Coordinate2D<double>
IntersectionRegionGraphController::computeNewPointOfInterest(double start,
                                                             double max,
                                                             bool stretch) {
  // Infer direction from start and max
  bool directionIsRight = start <= max;
  // Initialize best point of interest
  PointOfInterestData bestPoint{
      .coordinate =
          Coordinate2D<double>(directionIsRight ? INFINITY : -INFINITY, 0)};
  // Iterate on all active functions
  int numberOfActiveFunctions = functionStore()->numberOfActiveFunctions();
  assert(m_selectedRecordIndex >= 0 &&
         m_selectedRecordIndex < numberOfActiveFunctions);
  for (int i = 0; i < numberOfActiveFunctions; i++) {
    updateBestPointOfInterestForRecord(functionStore()->activeRecordAtIndex(i),
                                       start, max, stretch, directionIsRight,
                                       bestPoint);
  }

  if (bestPoint.record.isNull()) {
    // No further interest points.
    return PointOfInterest().xy();
  }
  // Apply best record
  m_selectedRecordIndex =
      functionStore()->indexOfRecordAmongActiveRecords(bestPoint.record);
  m_intersectedRecord = bestPoint.intersectedRecord;
  setRecord(bestPoint.record);
  App::app()->graphController()->setCursorIsRing(
      bestPoint.isUnreachedIntersection);
  return bestPoint.coordinate;
}

}  // namespace Graph
