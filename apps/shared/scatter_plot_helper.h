#ifndef SHARED_SCATTER_PLOT_HELPER_H
#define SHARED_SCATTER_PLOT_HELPER_H

#include <poincare/expression.h>
#include <poincare/list.h>
#include <poincare/point.h>

#include "poincare_helpers.h"

namespace Shared {

class ScatterPlotIterable {
  friend class ContinuousFunction;

  class Iterator {
   public:
    Iterator(Poincare::Expression expression, int i)
        : m_index(i), m_expression(expression) {}

    bool operator!=(const Iterator& other) const {
      assert(m_expression == other.m_expression);
      return m_index != other.m_index;
    }
    Iterator& operator++() {
      ++m_index;
      return *this;
    }
    Poincare::Point operator*() const {
      assert(m_index < listLength(m_expression));
      Poincare::Expression e =
          m_expression.type() == Poincare::ExpressionNode::Type::List
              ? m_expression.childAtIndex(m_index)
              : m_expression;
      assert(e.type() == Poincare::ExpressionNode::Type::Point);
      return static_cast<Poincare::Point&>(e);
    }

   private:
    int m_index;
    Poincare::Expression m_expression;
  };

 public:
  Iterator begin() const { return Iterator(m_expression, 0); }
  Iterator end() const {
    return Iterator(m_expression, listLength(m_expression));
  }

 private:
  ScatterPlotIterable(Poincare::Expression e) : m_expression(e) {
    assert(e.type() == Poincare::ExpressionNode::Type::Point ||
           (e.type() == Poincare::ExpressionNode::Type::List &&
            static_cast<Poincare::List&>(e).isListOfPoints(nullptr)));
  }

  static int listLength(const Poincare::Expression& e) {
    return e.type() == Poincare::ExpressionNode::Type::List
               ? e.numberOfChildren()
               : 1;
  }

  Poincare::Expression m_expression;
};

}  // namespace Shared

#endif
