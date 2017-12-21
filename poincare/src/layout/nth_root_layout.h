#ifndef POINCARE_NTH_ROOT_LAYOUT_H
#define POINCARE_NTH_ROOT_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class NthRootLayout : public StaticLayoutHierarchy<2> {
public:
  constexpr static KDCoordinate k_leftRadixHeight = 8;
  constexpr static KDCoordinate k_leftRadixWidth = 5;
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_rightRadixHeight = 2;
  constexpr static KDCoordinate k_radixHorizontalOverflow = 2;
  constexpr static KDCoordinate k_indexHeight = 5;
  constexpr static KDCoordinate k_heightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 1;
  constexpr static KDCoordinate k_radixLineThickness = 1;
  ExpressionLayout * radicandLayout();
  ExpressionLayout * indexLayout();
};

}

#endif
