#ifndef POINCARE_HELPERS_SCATTER_PLOT_ITERABLE_H
#define POINCARE_HELPERS_SCATTER_PLOT_ITERABLE_H

#include <poincare/coordinate_2D.h>
#include <poincare/expression.h>

namespace Poincare {

namespace Internal {
class Tree;
}

class ScatterPlotIterable {
 public:
  ScatterPlotIterable(const SystemExpression e);
  class Iterator {
   public:
    Iterator(const Internal::Tree* node) : m_node(node) {}
    Coordinate2D<float> operator*() const;
    bool operator!=(const Iterator& rhs) const;
    Iterator& operator++();

   private:
    const Internal::Tree* m_node;
  };

  Iterator begin() const;
  Iterator end() const;
  int length() const;

 private:
  const SystemExpression m_expression;
};

}  // namespace Poincare

#endif
