#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/dynamic_layout_hierarchy.h>

namespace Poincare {

class HorizontalLayout : public DynamicLayoutHierarchy {
public:
  using DynamicLayoutHierarchy::DynamicLayoutHierarchy;
  ExpressionLayout * clone() const override;

  /* Hierarchy */
  void replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild = true) override;
  void addOrMergeChildAtIndex(ExpressionLayout * eL, int index);

  /* Navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;

  /* Other */
  bool isHorizontal() const override { return true; }

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  void mergeChildrenAtIndex(ExpressionLayout * eL, int index); // WITHOUT delete.
private:
  bool moveVertically(ExpressionLayout::VerticalDirection direction, ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout);
  int indexOfChild(ExpressionLayout * eL) const;
};

}

#endif
