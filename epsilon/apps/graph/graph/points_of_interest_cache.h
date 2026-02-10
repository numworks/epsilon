#pragma once

#include <ion/storage/record.h>
#include <poincare/point_of_interest/point_of_interest_list.h>

namespace Graph {

class PointsOfInterestCache {
 public:
  PointsOfInterestCache(Ion::Storage::Record record)
      : m_record(record),
        m_checksum(0),
        m_start(NAN),
        m_end(NAN),
        m_computedStart(NAN),
        m_computedEnd(NAN),
        m_interestingPointsOverflowPool(false) {}
  PointsOfInterestCache() : PointsOfInterestCache(Ion::Storage::Record()) {}

  Ion::Storage::Record record() const { return m_record; }

  void setBounds(double start, double end);
  bool isFullyComputed() const {
    return m_interestingPointsOverflowPool ||
           (m_start == m_computedStart && m_end == m_computedEnd);
  }

  int numberOfPoints() const { return m_list.numberOfPoints(); }
  int numberOfPoints(Poincare::Interest interest) const;
  Poincare::PointOfInterest pointAtIndex(int i) const {
    return m_list.pointAtIndex(i);
  }

  bool computeUntilNthPoint(int n);
  // Return false it has been interrupted by the pool or the user (if allowed)
  bool computeNextStep(bool allowUserInterruptions);

  /* Return the first PointOfInterest between [xStart] and [xEnd]. Direction is
   * inferred from [xStart] and [xEnd] order. Use [interest] to filter the type
   * of PointOfInterest searched and a positive [subCurveIndex] to restrict the
   * search to a given subCurve. Use true [alongX] if next interest point should
   * be found along the X axis. Otherwise, the abscissa is used (which can be
   * the Y axis on some functions). */
  Poincare::PointOfInterest firstPointInDirection(
      double xStart, double xEnd, double yStart, bool stretch,
      Poincare::Interest interest = Poincare::Interest::None,
      int subCurveIndex = -1, bool alongX = false);

  /* Returns the point with the given interest at the given coordinates, or an
   * uninitialized point if there is no such point. If interest is None, any
   * point of interest at the given coordinates is returned. */
  Poincare::PointOfInterest pointOfInterestAtCoordinates(
      double x, double y,
      Poincare::Interest interest = Poincare::Interest::None) const;
  Poincare::PointOfInterest displayablePointOfInterestAtCoordinates(
      double x, double y,
      Poincare::Interest interest = Poincare::Interest::None) const;

  bool hasInterestAtCoordinates(
      double x, double y,
      Poincare::Interest interest = Poincare::Interest::None) const {
    return pointOfInterestAtCoordinates(x, y, interest).isInitialized();
  }
  bool hasDisplayableInterestAtCoordinates(
      double x, double y,
      Poincare::Interest interest = Poincare::Interest::None) const {
    return displayablePointOfInterestAtCoordinates(x, y, interest)
        .isInitialized();
  }

  bool canDisplayPoints(
      Poincare::Interest interest = Poincare::Interest::None) const {
    return !m_interestingPointsOverflowPool &&
           (numberOfPoints(interest) <= k_maxNumberOfDisplayablePoints);
  }

  static bool PointFitInterest(Poincare::PointOfInterest poi,
                               Poincare::Interest interest);

 private:
  constexpr static int k_maxNumberOfDisplayablePoints = 64;
  constexpr static double k_numberOfSteps = 25.0;

  double cacheStep() const;
  double solverStep() const;

  void stripOutOfBounds();
  Poincare::Expression computeBetween(double start, double end);
  void tidyDownstreamPoolFrom(const Poincare::PoolObject* treePoolCursor) const;

  Ion::Storage::Record
      m_record;  // This is not const because of the copy constructor
  uint32_t m_checksum;
  double m_start;
  double m_end;
  double m_computedStart;
  double m_computedEnd;
  Poincare::PointsOfInterestList m_list;
  bool m_interestingPointsOverflowPool;
};

}  // namespace Graph
