#include "scatter_plot_helper.h"

#include <poincare/src/memory/tree.h>

using namespace Poincare;

namespace Shared {

Point ScatterPlotIterable::Iterator::operator*() const {
  assert(m_node->isPoint());
  return Point::Builder(m_node->child(0), m_node->child(1));
}

bool ScatterPlotIterable::Iterator::operator!=(const Iterator& rhs) const {
  return m_node != rhs.m_node && !m_node->isUndefined();
}

ScatterPlotIterable::Iterator& ScatterPlotIterable::Iterator::operator++() {
  m_node = m_node->nextTree();
  return *this;
}

ScatterPlotIterable::Iterator ScatterPlotIterable::begin() const {
  return m_expression.isUndefined()
             ? end()
             : Iterator(m_expression.dimension().isListOfPoints()
                            ? m_expression.tree()->nextNode()
                            : m_expression.tree());
}

ScatterPlotIterable::Iterator ScatterPlotIterable::end() const {
  return Iterator(m_expression.tree()->nextTree());
}

int ScatterPlotIterable::length() const {
  return m_expression.isUndefined() ? 0
         : m_expression.dimension().isListOfPoints()
             ? m_expression.numberOfChildren()
             : 1;
}

ScatterPlotIterable::ScatterPlotIterable(const Poincare::SystemExpression e)
    : m_expression(e) {
  assert(e.dimension().isPointOrListOfPoints());
}

}  // namespace Shared
