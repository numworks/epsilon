#ifndef GRAPH_POINTS_OF_INTEREST_CACHE
#define GRAPH_POINTS_OF_INTEREST_CACHE

#include <ion/storage/record.h>
#include <poincare/numeric/point_of_interest.h>

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

  void setBounds(float start, float end);
  bool isFullyComputed() const {
    return m_interestingPointsOverflowPool ||
           (m_start == m_computedStart && m_end == m_computedEnd);
  }

  int numberOfPoints() const { return m_list.numberOfPoints(); }
  int numberOfPoints(Poincare::Solver<double>::Interest interest) const;
  Poincare::PointOfInterest pointAtIndex(int i) const {
    return m_list.pointAtIndex(i);
  }

  bool computeUntilNthPoint(int n);
  // Return false it has been interrupted by the pool or the user (if allowed)
  bool computeNextStep(bool allowUserInterruptions);

  Poincare::PointOfInterest firstPointInDirection(
      double start, double end, bool stretch,
      Poincare::Solver<double>::Interest interest =
          Poincare::Solver<double>::Interest::None,
      int subCurveIndex = 0);
  bool hasInterestAtCoordinates(
      double x, double y,
      Poincare::Solver<double>::Interest interest =
          Poincare::Solver<double>::Interest::None) const;
  bool hasDisplayableInterestAtCoordinates(
      double x, double y,
      Poincare::Solver<double>::Interest interest =
          Poincare::Solver<double>::Interest::None,
      bool allInterestsAreDisplayed = true) const;

  bool canDisplayPoints(Poincare::Solver<double>::Interest interest =
                            Poincare::Solver<double>::Interest::None) const {
    return !m_interestingPointsOverflowPool &&
           (numberOfPoints(interest) <= k_maxNumberOfDisplayablePoints);
  }

 private:
  constexpr static int k_maxNumberOfDisplayablePoints = 64;
  constexpr static float k_numberOfSteps = 25.0;

  float step() const;

  void stripOutOfBounds();
  Poincare::API::JuniorPoolHandle computeBetween(float start, float end);
  void tidyDownstreamPoolFrom(Poincare::PoolObject* treePoolCursor) const;

  Ion::Storage::Record
      m_record;  // This is not const because of the copy constructor
  uint32_t m_checksum;
  float m_start;
  float m_end;
  float m_computedStart;
  float m_computedEnd;
  Poincare::PointsOfInterestList m_list;
  bool m_interestingPointsOverflowPool;
};

}  // namespace Graph

#endif
