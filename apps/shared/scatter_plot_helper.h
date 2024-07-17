#ifndef SHARED_SCATTER_PLOT_HELPER_H
#define SHARED_SCATTER_PLOT_HELPER_H

#include <poincare/expression.h>

namespace Poincare::Internal {
class Tree;
}

namespace Shared {

class ScatterPlotIterable {
  friend class ContinuousFunction;

 public:
  class Iterator {
   public:
    Iterator(const Poincare::Internal::Tree* node) : m_node(node) {}
    Poincare::Point operator*() const;
    bool operator!=(const Iterator& rhs) const;
    Iterator& operator++();

   private:
    const Poincare::Internal::Tree* m_node;
  };

  Iterator begin() const;
  Iterator end() const;
  int length() const;

 private:
  ScatterPlotIterable(const Poincare::SystemExpression e);

  const Poincare::SystemExpression m_expression;
};

}  // namespace Shared

#endif
