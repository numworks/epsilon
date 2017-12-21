#ifndef POINCARE_SUM_LAYOUT_H
#define POINCARE_SUM_LAYOUT_H

#include "sequence_layout.h"

namespace Poincare {

class SumLayout : public SequenceLayout {
public:
  SumLayout(ExpressionLayout * lowerBound, ExpressionLayout * upperBound, ExpressionLayout * argument, bool cloneOperands);
  ExpressionLayout * clone() const override;
  ExpressionLayout * argumentLayout() override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
};

}

#endif
