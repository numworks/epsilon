#ifndef POINCARE_INTEGRAL_LAYOUT_H
#define POINCARE_INTEGRAL_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class IntegralLayout : public StaticLayoutHierarchy<3> {
public:
  constexpr static KDCoordinate k_symbolHeight = 4;
  constexpr static KDCoordinate k_symbolWidth = 4;
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;

  /* Dynamic Layout*/
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;

  /* Tree navigation */
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;

  /* Expression Engine */
  int writeTextInBuffer(char * buffer, int bufferSize) const override;

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_boundHeightMargin = 8;
  constexpr static KDCoordinate k_boundWidthMargin = 5;
  constexpr static KDCoordinate k_integrandWidthMargin = 2;
  constexpr static KDCoordinate k_integrandHeigthMargin = 2;
  constexpr static KDCoordinate k_lineThickness = 1;
  ExpressionLayout * lowerBoundLayout();
  ExpressionLayout * upperBoundLayout();
  ExpressionLayout * integrandLayout();
};

}

#endif
