#ifndef POINCARE_NTH_ROOT_LAYOUT_H
#define POINCARE_NTH_ROOT_LAYOUT_H

#include <poincare/bounded_static_layout_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class NthRootLayout : public BoundedStaticLayoutHierarchy<2> {
public:
  constexpr static KDCoordinate k_leftRadixHeight = 8;
  constexpr static KDCoordinate k_leftRadixWidth = 5;
  using BoundedStaticLayoutHierarchy::BoundedStaticLayoutHierarchy;
  ExpressionLayout * clone() const override;

  /* Dynamic Layout*/
  void collapseBrothers() override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;

  /* Tree navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;

  /* Expression Engine */
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_rightRadixHeight = 2;
  constexpr static KDCoordinate k_radixHorizontalOverflow = 2;
  constexpr static KDCoordinate k_indexHeight = 4;
  constexpr static KDCoordinate k_heightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 1;
  constexpr static KDCoordinate k_radixLineThickness = 1;
  ExpressionLayout * radicandLayout();
  ExpressionLayout * indexLayout();
};

}

#endif
