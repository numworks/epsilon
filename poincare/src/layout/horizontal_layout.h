#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/dynamic_layout_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/expression_layout_cursor.h>

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
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;

  /* Dynamic layout */
  void addChildrenAtIndex(const ExpressionLayout * const * operands, int numberOfOperands, int indexForInsertion, bool removeEmptyChildren) override;
  bool addChildAtIndex(ExpressionLayout * operand, int index) override;
  void removeChildAtIndex(int index, bool deleteAfterRemoval) override;
  void mergeChildrenAtIndex(DynamicLayoutHierarchy * eL, int index, bool removeEmptyChildren) override;

  /* Expression Engine */
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;

  /* Other */
  bool isHorizontal() const override { return true; }
  bool isEmpty() const override;
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  void privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor) override;
private:
  bool moveVertically(ExpressionLayout::VerticalDirection direction, ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout);
  bool tryMoveVerticallyFromAnotherLayout(ExpressionLayout * otherLayout, ExpressionLayoutCursor::Position otherPosition, ExpressionLayout::VerticalDirection direction, ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout);
  void privateReplaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor);
  void privateRemoveChildAtIndex(int index, bool deleteAfterRemoval, bool forceRemove);
  int removeEmptyChildBeforeInsertionAtIndex(int index, bool shouldRemoveOnLeft);
};

}

#endif
