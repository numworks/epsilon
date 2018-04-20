#ifndef POINCARE_MATRIX_LAYOUT_H
#define POINCARE_MATRIX_LAYOUT_H

#include <poincare/src/layout/grid_layout.h>

namespace Poincare {

class MatrixLayout : public GridLayout {
public:
  using GridLayout::GridLayout;
  ExpressionLayout * clone() const override;

  /* Navigation */
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorAbove(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  ExpressionLayoutCursor cursorUnder(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  ExpressionLayoutCursor cursorInDescendantsAbove(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorInDescendantsUnder(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;

  /* Dynamic layout */
  void replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild) override;
  void replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) override;
  void removePointedChildAtIndexAndMoveCursor(int index, bool deleteAfterRemoval, ExpressionLayoutCursor * cursor) override;

  /* Expression engine */
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;

  /* Other */
  bool isMatrix() const override { return true; }

  /* Special matrix method */
  void newRowOrColumnAtIndex(int index);
  void addGreySquares();

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  void childWasReplacedAtIndex(int index);
  bool isRowEmpty(int index) const;
  bool isColumnEmpty(int index) const;
  void removeGreySquares();
  bool hasGreySquares() const;
};

}

#endif
