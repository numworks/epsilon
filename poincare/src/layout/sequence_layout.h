#ifndef POINCARE_SEQUENCE_LAYOUT_H
#define POINCARE_SEQUENCE_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class SequenceLayout : public StaticLayoutHierarchy<3> {
public:
  constexpr static KDCoordinate k_symbolHeight = 15;
  constexpr static KDCoordinate k_symbolWidth = 9;
  SequenceLayout(ExpressionLayout * lowerBound, ExpressionLayout * upperBound, ExpressionLayout * argument, bool cloneOperands);
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  char XNTChar() const override;
protected:
  constexpr static KDCoordinate k_boundHeightMargin = 2;
  constexpr static KDCoordinate k_argumentWidthMargin = 2;
  int writeDerivedClassInBuffer(const char * operatorName, char * buffer, int bufferSize) const;
  ExpressionLayout * lowerBoundLayout();
  ExpressionLayout * upperBoundLayout();
  ExpressionLayout * argumentLayout();
  ExpressionLayout * argumentWithParenthesesLayout();
  KDSize computeSize() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  void computeBaseline() override;
};

}

#endif
