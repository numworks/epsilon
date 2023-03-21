#ifndef SHARED_SCATTER_PLOT_HELPER_H
#define SHARED_SCATTER_PLOT_HELPER_H

#include <poincare/expression.h>
#include <poincare/list.h>
#include <poincare/point.h>

#include "poincare_helpers.h"

namespace Shared {

class ScatterPlotIterable {
  friend class ContinuousFunction;

 protected:
  ScatterPlotIterable(Poincare::Expression e) {
    if (e.type() == Poincare::ExpressionNode::Type::List) {
      m_list = static_cast<Poincare::List&>(e);
    } else {
      m_list = Poincare::List::Builder();
      m_list.addChildAtIndexInPlace(e, 0, 0);
    }
    assert(m_list.isListOfPoints(nullptr));
  }

  class Iterator {
   public:
    Iterator(Poincare::List list, int i) : m_index(i), m_list(list) {}

    bool operator!=(const Iterator& other) const {
      assert(m_list == other.m_list);
      return m_index != other.m_index;
    }
    Iterator& operator++() {
      ++m_index;
      return *this;
    }
    Poincare::Point operator*() const {
      assert(m_index < m_list.numberOfChildren());
      Poincare::Expression e = m_list.childAtIndex(m_index);
      assert(e.type() == Poincare::ExpressionNode::Type::Point);
      return static_cast<Poincare::Point&>(e);
    }

   private:
    int m_index;
    Poincare::List m_list;
  };

  Iterator begin() const { return Iterator(m_list, 0); }
  Iterator end() const { return Iterator(m_list, m_list.numberOfChildren()); }

  Poincare::List m_list;
};

}  // namespace Shared

#endif
