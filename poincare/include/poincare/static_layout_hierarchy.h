#ifndef POINCARE_STATIC_LAYOUT_HIERARCHY_H
#define POINCARE_STATIC_LAYOUT_HIERARCHY_H

#include <poincare/expression_layout.h>

namespace Poincare {

template<int T>
class StaticLayoutHierarchy : public ExpressionLayout {
public:
  StaticLayoutHierarchy();
  StaticLayoutHierarchy(const ExpressionLayout * const * operands, bool cloneOperands = true);
  StaticLayoutHierarchy(const ExpressionLayout * expression, bool cloneOperands = true); // Specialized constructor for StaticLayoutHierarchy<1>
  StaticLayoutHierarchy(const ExpressionLayout * expression1, const ExpressionLayout * expression2, bool cloneOperands = true); // Specialized constructor for StaticLayoutHierarchy<2>
  StaticLayoutHierarchy(const ExpressionLayout * expression1, const ExpressionLayout * expression2, const ExpressionLayout * expression3, bool cloneOperands = true); // Specialized constructor for StaticLayoutHierarchy<3>
  ~StaticLayoutHierarchy();
  StaticLayoutHierarchy(const StaticLayoutHierarchy & other) = delete;
  StaticLayoutHierarchy(StaticLayoutHierarchy && other) = delete;
  StaticLayoutHierarchy& operator=(const StaticLayoutHierarchy & other) = delete;
  StaticLayoutHierarchy& operator=(StaticLayoutHierarchy && other) = delete;

  int numberOfChildren() const override { return T; }
  const ExpressionLayout * const * children() const override { return m_children; }
protected:
  void build(const ExpressionLayout * const * operands, int numberOfOperands, bool cloneOperands);
  const ExpressionLayout * m_children[T];
};

}

#endif
