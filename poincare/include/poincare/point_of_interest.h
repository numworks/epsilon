#ifndef POINCARE_POINT_OF_INTEREST_H
#define POINCARE_POINT_OF_INTEREST_H

#include <poincare/list.h>
#include <poincare/solver.h>

namespace Poincare {

class PointOfInterestNode : public TreeNode {
  friend class PointOfInterest;
public:
  PointOfInterestNode(double abscissa, double ordinate, typename Solver<double>::Interest interest, uint32_t data, bool inverted) : m_abscissa(abscissa), m_ordinate(ordinate), m_data(data), m_interest(interest), m_inverted(inverted) {}

  // TreeNode
  size_t size() const override { return sizeof(PointOfInterestNode); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "PointOfInterest"; }
  void logAttributes(std::ostream & stream) const override { stream << " abscissa=\"" << m_abscissa << "\" ordinate=\"" << m_ordinate << "\""; }
#endif

private:
  double m_abscissa;
  double m_ordinate;
  uint32_t m_data;
  typename Solver<double>::Interest m_interest : 7;
  bool m_inverted : 1;
};

class PointOfInterest : public TreeHandle {
public:
  static PointOfInterest Builder(double abscissa, double ordinate, typename Solver<double>::Interest interest, uint32_t data, bool inverted);

  PointOfInterest(PointOfInterestNode * node = nullptr) : TreeHandle(node) {}

  /* Abscissa/ordinate are from the function perspective, while x/y are related
   * to the drawings. They differ only with functions along y. */
  double abscissa() const { return node()->m_abscissa; }
  double ordinate() const { return node()->m_ordinate; }
  double x() const { return node()->m_inverted ? node()->m_ordinate : node()->m_abscissa; }
  double y() const { return node()->m_inverted ? node()->m_abscissa : node()->m_ordinate; }
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
  void setList(List list) { m_list = list; }
  bool isUninitialized() const { return m_list.isUninitialized(); }
  int numberOfPoints() const { return m_list.numberOfChildren(); }
  Poincare::PointOfInterest pointAtIndex(int i) const;
  void append(double abscissa, double ordinate, uint32_t data, typename Solver<double>::Interest interest, bool inverted);
  void sort();

private:
  List m_list;
};

}

#endif
