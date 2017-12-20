#ifndef POINCARE_SEQUENCE_LAYOUT_H
#define POINCARE_SEQUENCE_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class SequenceLayout : public StaticLayoutHierarchy<3> {
public:
  constexpr static KDCoordinate k_symbolHeight = 15;
  constexpr static KDCoordinate k_symbolWidth = 9;
  SequenceLayout(ExpressionLayout * lowerBound, ExpressionLayout * upperBound, ExpressionLayout * argument, bool cloneOperands);
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
protected:
  constexpr static KDCoordinate k_boundHeightMargin = 2;
  ExpressionLayout * lowerBoundLayout();
  ExpressionLayout * upperBoundLayout();
  ExpressionLayout * argumentLayout();
private:
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  constexpr static KDCoordinate k_argumentWidthMargin = 2;
};

}

#endif
