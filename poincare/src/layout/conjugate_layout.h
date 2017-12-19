#ifndef POINCARE_CONJUGATE_LAYOUT_H
#define POINCARE_CONJUGATE_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class ConjugateLayout : public StaticLayoutHierarchy<1> {
public:
  ConjugateLayout(ExpressionLayout * operand, bool cloneOperands);
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_overlineWidth = 1;
  constexpr static KDCoordinate k_overlineMargin = 3;
  ExpressionLayout * operandLayout();
};

}

#endif

