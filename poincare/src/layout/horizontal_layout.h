#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/dynamic_layout_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class HorizontalLayout : public DynamicLayoutHierarchy {
  friend class BinomialCoefficientLayout;
  friend class IntegralLayout;
  friend class MatrixLayout;
  friend class SequenceLayout;
public:
  using DynamicLayoutHierarchy::DynamicLayoutHierarchy;
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;

  /* Hierarchy */
  void replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild) override;
  void replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) override;
  void addOrMergeChildAtIndex(ExpressionLayout * eL, int index, bool removeEmptyChildren);

  /* Navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr) override;
  bool moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;

  /* Dynamic layout */
  void removeChildAtIndex(int index, bool deleteAfterRemoval) override;

  /* Expression Engine */
  int writeTextInBuffer(char * buffer, int bufferSize) const override;

  /* Other */
  bool isHorizontal() const override { return true; }
  bool isEmpty() const override;

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  bool moveVertically(ExpressionLayout::VerticalDirection direction, ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout);
  void privateReplaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor);
};

}

#endif
