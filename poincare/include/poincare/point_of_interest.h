#ifndef POINCARE_POINT_OF_INTEREST_H
#define POINCARE_POINT_OF_INTEREST_H

#include <poincare/list.h>
#include <poincare/solver.h>

namespace Poincare {

class PointOfInterestNode : public TreeNode {
  friend class PointOfInterest;
public:
  PointOfInterestNode(double x, double y, typename Solver<double>::Interest interest, uint32_t data) : m_x(x), m_y(y), m_data(data), m_interest(interest) {}

  // TreeNode
  size_t size() const override { return sizeof(PointOfInterestNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "PointOfInterest"; }
  void logAttributes(std::ostream & stream) const override { stream << " x=\"" << m_x << "\" y=\"" << m_y << "\""; }
#endif

private:
  double m_x;
  double m_y;
  uint32_t m_data;
  typename Solver<double>::Interest m_interest;
};

class PointOfInterest : public TreeHandle {
public:
  static PointOfInterest Builder(double x, double y, typename Solver<double>::Interest interest, uint32_t data);

  PointOfInterest() : TreeHandle(nullptr) {}
  PointOfInterest(PointOfInterestNode * node) : TreeHandle(node) {}

  double x() const { return node()->m_x; }
  double y() const { return node()->m_y; }
  typename Solver<double>::Interest interest() const { return node()->m_interest; }
  Coordinate2D<double> xy() const { return isUninitialized() ? Coordinate2D<double>() : Coordinate2D<double>(x(), y()); }
  uint32_t data() const { return node()->m_data; }

private:
  PointOfInterestNode * node() const { return static_cast<PointOfInterestNode *>(TreeHandle::node()); }
};

class PointsOfInterestList {
public:
  List list() const { return m_list; }
  void init() { m_list = List::Builder(); }
  bool isUninitialized() const { return m_list.isUninitialized(); }
  int numberOfPoints() const { return m_list.numberOfChildren(); }
  Poincare::PointOfInterest pointAtIndex(int i) const;
  void append(double x, double y, uint32_t data, typename Solver<double>::Interest interest);
  void sort();

private:
  List m_list;
};

}

#endif
