#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/dynamic_layout_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class HorizontalLayout : public DynamicLayoutHierarchy {
  friend class IntegralLayout;
public:
  using DynamicLayoutHierarchy::DynamicLayoutHierarchy;
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;

  /* Hierarchy */
  void replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild = true) override;
  void addOrMergeChildAtIndex(ExpressionLayout * eL, int index);

  /* Navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;

  /* Dynamic layout */
  void removeChildAtIndex(int index, bool deleteAfterRemoval) override;

  /* Expression Engine */
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writeInfixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "");
  }

  /* Other */
  bool isHorizontal() const override { return true; }

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  bool moveVertically(ExpressionLayout::VerticalDirection direction, ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout);
};

}

#endif
