#ifndef POINCARE_POINT_OF_INTEREST_H
#define POINCARE_POINT_OF_INTEREST_H

#include <poincare/solver.h>

namespace Poincare {

class PointOfInterest;

class PointOfInterestNode : public TreeNode {
  friend class PointOfInterest;
public:
  PointOfInterestNode(double x, double y, Solver<double>::Interest interest) : m_x(x), m_y(y), m_interest(interest) {}

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
  Solver<double>::Interest m_interest;
};

class PointOfInterest : public TreeHandle {
public:
  static PointOfInterest Builder(double x, double y, Solver<double>::Interest interest);

  PointOfInterest(PointOfInterestNode * node) : TreeHandle(node) {}

  double x() const { return node()->m_x; }
  double y() const { return node()->m_y; }
  Solver<double>::Interest interest() const { return node()->m_interest; }
  Coordinate2D<double> xy() const { return isUninitialized() ? Coordinate2D<double>() : Coordinate2D<double>(x(), y()); }

private:
  PointOfInterestNode * node() const { return static_cast<PointOfInterestNode *>(TreeHandle::node()); }
};

}

#endif
