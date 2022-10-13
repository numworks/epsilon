#ifndef POINCARE_POINT_OF_INTEREST_H
#define POINCARE_POINT_OF_INTEREST_H

#include <poincare/list.h>
#include <poincare/solver.h>

namespace Poincare {

template<typename T> class PointOfInterest;

template<typename T>
class PointOfInterestNode : public TreeNode {
  friend class PointOfInterest<T>;
public:
  PointOfInterestNode(T x, T y, typename Solver<T>::Interest interest, uint32_t data) : m_x(x), m_y(y), m_data(data), m_interest(interest) {}

  // TreeNode
  size_t size() const override { return sizeof(PointOfInterestNode<T>); }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "PointOfInterest"; }
  void logAttributes(std::ostream & stream) const override { stream << " x=\"" << m_x << "\" y=\"" << m_y << "\""; }
#endif

private:
  T m_x;
  T m_y;
  uint32_t m_data;
  typename Solver<T>::Interest m_interest;
};

template<typename T>
class PointOfInterest : public TreeHandle {
public:
  static PointOfInterest Builder(T x, T y, typename Solver<T>::Interest interest, uint32_t data);

  PointOfInterest(PointOfInterestNode<T> * node) : TreeHandle(node) {}

  T x() const { return node()->m_x; }
  T y() const { return node()->m_y; }
  typename Solver<T>::Interest interest() const { return node()->m_interest; }
  Coordinate2D<T> xy() const { return isUninitialized() ? Coordinate2D<T>() : Coordinate2D<T>(x(), y()); }
  uint32_t data() const { return node()->m_data; }

private:
  PointOfInterestNode<T> * node() const { return static_cast<PointOfInterestNode<T> *>(TreeHandle::node()); }
};

template<typename T>
class PointsOfInterestList {
public:
  List list() const { return m_list; }
  void init() { m_list = List::Builder(); }
  bool isUninitialized() const { return m_list.isUninitialized(); }
  int numberOfPoints() const { return m_list.numberOfChildren(); }
  Poincare::PointOfInterest<T> pointAtIndex(int i) const;
  void append(T x, T y, uint32_t data, typename Solver<T>::Interest interest);

private:
  List m_list;
};

}

#endif
