#ifndef SHARED_SCATTER_PLOT_HELPER_H
#define SHARED_SCATTER_PLOT_HELPER_H

#include <poincare/expression.h>
#include <poincare/list.h>
#include <poincare/list_sort.h>
#include <poincare/old_expression.h>
#include <poincare/point.h>

#include "poincare_helpers.h"

namespace Shared {

class ScatterPlotIterable {
  friend class ContinuousFunction;

  using ExpressionIterable =
      Poincare::TreeHandle::Direct<Poincare::Expression,
                                   Poincare::ExpressionNode>;

  class Iterator : public ExpressionIterable::Iterator {
   public:
    Iterator(ExpressionIterable::Iterator iter)
        : ExpressionIterable::Iterator(iter) {}
    using ExpressionIterable::Iterator::Iterator;
    Poincare::Point operator*() const {
      Poincare::Expression e = ExpressionIterable::Iterator::operator*();
      assert(e.type() == Poincare::ExpressionNode::Type::Point);
      return static_cast<Poincare::Point&>(e);
    }
    bool operator!=(const Iterator& rhs) const {
      return this->ExpressionIterable::Iterator::operator!=(rhs) &&
             !ExpressionIterable::Iterator::operator*().isUndefined();
    }
  };

 public:
  Iterator begin() const {
    return m_expression.type() == Poincare::ExpressionNode::Type::Point
               ? Iterator(m_iterable.node())
           : m_expression.isUndefined() ? end()
                                        : m_iterable.begin();
  }
  Iterator end() const { return m_iterable.end(); }

  int length() const { return ListLength(m_expression); }

 private:
  ScatterPlotIterable(Poincare::Expression e) : m_iterable(e), m_expression(e) {
    assert(Poincare::Expression::IsPoint(e) ||
           (e.type() == Poincare::ExpressionNode::Type::List &&
            static_cast<Poincare::List&>(e).isListOfPoints(nullptr)));
  }

  static int ListLength(const Poincare::Expression& e) {
    return e.type() == Poincare::ExpressionNode::Type::List
               ? e.numberOfChildren()
           : e.isUndefined() ? 0
                             : 1;
  }

  ExpressionIterable m_iterable;
  Poincare::Expression m_expression;
};

}  // namespace Shared

#endif
