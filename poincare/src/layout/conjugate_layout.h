#ifndef POINCARE_CONJUGATE_LAYOUT_H
#define POINCARE_CONJUGATE_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class ConjugateLayout : public StaticLayoutHierarchy<1> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_overlineWidth = 1;
  constexpr static KDCoordinate k_overlineVerticalMargin = 1;
  ExpressionLayout * operandLayout();
};

}

#endif

