#ifndef GRAPH_POINTS_OF_INTEREST_CACHE
#define GRAPH_POINTS_OF_INTEREST_CACHE

#include <poincare/point_of_interest.h>
#include <poincare/range.h>
#include <ion/storage/record.h>

namespace Graph {

class PointsOfInterestCache {
public:
  PointsOfInterestCache(Ion::Storage::Record record) : m_record(record), m_start(NAN), m_end(NAN) {}
  PointsOfInterestCache() : PointsOfInterestCache(Ion::Storage::Record()) {}

  Poincare::List list() { return m_list.list(); }
  Ion::Storage::Record record() const { return m_record; }
  PointsOfInterestCache clone() const;

  void setBounds(float start, float end);
  bool isFullyComputed() const { return m_start == m_computedStart && m_end == m_computedEnd; }

  int numberOfPoints() const { return m_list.numberOfPoints(); }
  int numberOfPoints(Poincare::Solver<double>::Interest interest) const;
  Poincare::PointOfInterest pointAtIndex(int i) const { return m_list.pointAtIndex(i); }

  bool computeUntilNthPoint(int n);
  void computeNextStep();

  Poincare::PointOfInterest firstPointInDirection(double start, double end, Poincare::Solver<double>::Interest interest = Poincare::Solver<double>::Interest::None);
  bool hasInterestAtCoordinates(double x, double y, Poincare::Solver<double>::Interest interest = Poincare::Solver<double>::Interest::None);

private:
  constexpr static float k_numberOfSteps = 25.0;
  float step() const { return (m_end - m_start) / k_numberOfSteps; }

  void stripOutOfBounds();
  void computeBetween(float start, float end);
  void append(double x, double y, Poincare::Solver<double>::Interest, uint32_t data = 0);

  Ion::Storage::Record m_record; // This is not const because of the copy constructor
  uint32_t m_checksum;
  float m_start;
  float m_end;
  float m_computedStart;
  float m_computedEnd;
  Poincare::PointsOfInterestList m_list;
};

}

#endif
