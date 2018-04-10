#ifndef POINCARE_MATRIX_LAYOUT_H
#define POINCARE_MATRIX_LAYOUT_H

#include <poincare/src/layout/grid_layout.h>

namespace Poincare {

class MatrixLayout : public GridLayout {
public:
  using GridLayout::GridLayout;
  ExpressionLayout * clone() const override;

  /* Navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr, ExpressionLayout * previousLayout = nullptr, ExpressionLayout * previousPreviousLayout = nullptr) override;
  bool moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr, ExpressionLayout * previousLayout = nullptr, ExpressionLayout * previousPreviousLayout = nullptr) override;
  bool moveUpInside(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveDownInside(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;

  /* Dynamic layout */
  void replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild) override;
  void replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) override;
  void removePointedChildAtIndexAndMoveCursor(int index, bool deleteAfterRemoval, ExpressionLayoutCursor * cursor) override;

  /* Expression engine */
  int writeTextInBuffer(char * buffer, int bufferSize) const override;

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
