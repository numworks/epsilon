#include "intersection_region_graph_controller.h"

#include "../app.h"

using namespace Poincare;

namespace Graph {

Coordinate2D<double>
IntersectionRegionGraphController::computeNewPointOfInterest(double start,
                                                             double max,
                                                             bool stretch) {
  // Infer direction from start and max
  bool directionIsRight = start <= max;
  // Find the next point of interest among all active inequalities
  Coordinate2D<double> bestCoordinate(directionIsRight ? INFINITY : -INFINITY,
                                      0);
  bool bestIsUnreachedIntersection = false;
  Ion::Storage::Record bestRecord;
  // Iterate on all active inequalities
  int numberOfActiveFunctions = functionStore()->numberOfActiveFunctions();
  assert(m_selectedRecordIndex >= 0 &&
         m_selectedRecordIndex < numberOfActiveFunctions);
  for (int i = 0; i < numberOfActiveFunctions; i++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(i);
    if (functionStore()->modelForRecord(record)->properties().isEquality()) {
      continue;
    }
    PointOfInterest p = computeAtLeastOnePointOfInterest(
        App::app()->graphController()->pointsOfInterestForRecord(record), start,
        max, stretch);
    if (p.isUninitialized()) {
      // No point of interest for this record
      continue;
    }
    /* TODO: multiple points can be at the same abscissa, we could also compare
     * y here but it won't work anyway because in next iteration, interest
     * points at exactly start will be ignored. */
    if (!(directionIsRight && p.x() < bestCoordinate.x()) &&
        !(!directionIsRight && p.x() > bestCoordinate.x())) {
      // Point of interest isn't the next one
      continue;
    }
    Ion::Storage::Record intersectedRecord = Ion::Storage::Record(p.data);
    int intersectedRecordIndex =
        functionStore()->indexOfRecordAmongActiveRecords(intersectedRecord);
    assert(intersectedRecordIndex != i);
    if (intersectedRecordIndex <= i) {
      // Intersected record has already been iterated, skip the intersection.
      continue;
    }
    if (functionStore()
            ->modelForRecord(intersectedRecord)
            ->properties()
            .isEquality()) {
      // Only display intersections with inequalities
      continue;
    }
    // Update best point of interest
    bestCoordinate = p.xy();
    bestIsUnreachedIntersection =
        (p.interest == Solver<double>::Interest::UnreachedIntersection);
    bestRecord = record;
    m_intersectedRecord = intersectedRecord;
    m_selectedRecordIndex = i;
  }

  if (bestRecord.isNull()) {
    // No further interest points.
    return PointOfInterest().xy();
  }
  // Apply best record
  setRecord(bestRecord);
  App::app()->graphController()->setCursorIsRing(bestIsUnreachedIntersection);
  return bestCoordinate;
}

}  // namespace Graph
