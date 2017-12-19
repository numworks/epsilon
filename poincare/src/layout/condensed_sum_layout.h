#ifndef POINCARE_CONDENSED_SUM_LAYOUT_H
#define POINCARE_CONDENSED_SUM_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class CondensedSumLayout : public StaticLayoutHierarchy<3> {
public:
  CondensedSumLayout(ExpressionLayout * base, ExpressionLayout * subscript, ExpressionLayout * superscript, bool cloneOperands);
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  bool moveUp(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  ExpressionLayout * baseLayout();
  ExpressionLayout * subscriptLayout();
  ExpressionLayout * superscriptLayout();
};

}

#endif

