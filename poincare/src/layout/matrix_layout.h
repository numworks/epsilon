#ifndef POINCARE_MATRIX_LAYOUT_H
#define POINCARE_MATRIX_LAYOUT_H

#include <poincare/src/layout/grid_layout.h>

namespace Poincare {

class MatrixLayout : public GridLayout {
public:
  using GridLayout::GridLayout;
  ExpressionLayout * clone() const override;
  void replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild = true) override;
  void replaceChildAndMoveCursor(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild, ExpressionLayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  bool isMatrix() const override { return true; }
  void newRowOrColumnAtIndex(int index);
  void childWasReplacedAtIndex(int index);
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  bool isRowEmpty(int index) const;
  bool isColumnEmpty(int index) const;
};

}

#endif
