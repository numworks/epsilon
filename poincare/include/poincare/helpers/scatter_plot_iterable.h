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
  ScatterPlotIterable(const Poincare::SystemExpression e);
  class Iterator {
   public:
    Iterator(const Poincare::Internal::Tree* node) : m_node(node) {}
    Poincare::Coordinate2D<float> operator*() const;
    bool operator!=(const Iterator& rhs) const;
    Iterator& operator++();

   private:
    const Poincare::Internal::Tree* m_node;
  };

  Iterator begin() const;
  Iterator end() const;
  int length() const;

 private:
  const Poincare::SystemExpression m_expression;
};

}  // namespace Poincare

#endif
