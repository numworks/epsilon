#ifndef GRAPH_POINTS_OF_INTEREST_LIST
#define GRAPH_POINTS_OF_INTEREST_LIST

#include <poincare/list.h>
#include <poincare/point_of_interest.h>
#include <ion/storage/record.h>

namespace Graph {

class PointsOfInterestList {
public:
  PointsOfInterestList() : m_start(NAN), m_end(NAN) {}

  bool isUpToDate() const { return m_record.checksum() == m_checksum; }
  void setRecord(Ion::Storage::Record record) { m_record = record; }
  void setBoundsAndCompute(float start, float end);
  Poincare::PointOfInterest pointAtIndex(int i) const;

private:
  void stripOutOfBounds();
  void computeBetween(float start, float end);
  void append(double x, double y, Poincare::Solver<double>::Interest);

  Ion::Storage::Record m_record;
  uint32_t m_checksum;
  float m_start;
  float m_end;
  Poincare::List m_list;
};

}

#endif
