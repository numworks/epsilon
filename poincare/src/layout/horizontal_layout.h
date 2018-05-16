#ifndef POINCARE_HORIZONTAL_LAYOUT_H
#define POINCARE_HORIZONTAL_LAYOUT_H

#include <poincare/dynamic_layout_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/expression_layout_cursor.h>

namespace Poincare {

/* WARNING: A Horizontal Layout should never have a Horizontal Layout child.
 * For instance, use addOrMergeChildAtIndex to add an ExpressionLayout safely. */

class HorizontalLayout : public DynamicLayoutHierarchy {
  friend class BinomialCoefficientLayout;
  friend class IntegralLayout;
  friend class MatrixLayout;
  friend class SequenceLayout;
public:
  using DynamicLayoutHierarchy::DynamicLayoutHierarchy;
  ExpressionLayout * clone() const override;
  void deleteBeforeCursor(ExpressionLayoutCursor * cursor) override;

  // Replace
  void replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild) override;
  void replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) override;
  void addOrMergeChildAtIndex(ExpressionLayout * eL, int index, bool removeEmptyChildren);

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;

  // Dynamic layout
  void addChildrenAtIndex(const ExpressionLayout * const * operands, int numberOfOperands, int indexForInsertion, bool removeEmptyChildren) override;
  bool addChildAtIndex(ExpressionLayout * operand, int index) override;
  void removeChildAtIndex(int index, bool deleteAfterRemoval) override;
  void mergeChildrenAtIndex(DynamicLayoutHierarchy * eL, int index, bool removeEmptyChildren) override;

  // Serialization
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;

  // Cursor
  ExpressionLayoutCursor equivalentCursor(ExpressionLayoutCursor cursor) override;

  // Other
  bool isHorizontal() const override { return true; }
  bool isEmpty() const override { return m_numberOfChildren == 1 && child(0)->isEmpty(); }
  bool needsParenthesesWithParent() const override;
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override { return m_numberOfChildren != 0; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  void privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor) override;
private:
  void privateReplaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor);
  void privateRemoveChildAtIndex(int index, bool deleteAfterRemoval, bool forceRemove);
  int removeEmptyChildBeforeInsertionAtIndex(int index, bool shouldRemoveOnLeft);
};

}

#endif
