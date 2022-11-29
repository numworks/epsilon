#ifndef GRAPH_POINTS_OF_INTEREST_CACHE
#define GRAPH_POINTS_OF_INTEREST_CACHE

#include <poincare/point_of_interest.h>
#include <poincare/range.h>
#include <ion/storage/record.h>

namespace Graph {

class PointsOfInterestCache {
public:
  PointsOfInterestCache() : m_start(NAN), m_end(NAN) {}

  Poincare::List list() { return m_list.list(); }
  void setRecord(Ion::Storage::Record record) { m_record = record; }
  Ion::Storage::Record currentRecord() const { return m_record; }
  PointsOfInterestCache clone() const;

  Poincare::Range1D setBounds(float start, float end);
  void computeNextStep();
  bool isFullyComputed() const { return m_start == m_computedStart && m_end == m_computedEnd; }

  int numberOfPoints() const { return m_list.numberOfPoints(); }
  Poincare::PointOfInterest pointAtIndex(int i) const { return m_list.pointAtIndex(i); }

  Poincare::PointOfInterest firstPointInDirection(double start, double end, Poincare::Solver<double>::Interest interest = Poincare::Solver<double>::Interest::None);
  Poincare::PointOfInterest pointOfInterestAtAbscissa(double x);

  float step() const { return (m_end - m_start) / k_numberOfSteps; }
private:
  constexpr static float k_numberOfSteps = 25.0;

  void stripOutOfBounds();
  void computeBetween(float start, float end);
  void append(double x, double y, Poincare::Solver<double>::Interest, uint32_t data = 0);

  Ion::Storage::Record m_record;
  Ion::Storage::Record m_computedRecord;
  uint32_t m_checksum;
  float m_start;
  float m_end;
  float m_computedStart;
  float m_computedEnd;
  Poincare::PointsOfInterestList m_list;
};

}

#endif
