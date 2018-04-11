#ifndef POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_H
#define POINCARE_BINOMIAL_COEFFICIENT_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/layout_engine.h>
#include <poincare/print_float.h>

namespace Poincare {

class BinomialCoefficientLayout : public StaticLayoutHierarchy<2> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr, ExpressionLayout * previousLayout = nullptr, ExpressionLayout * previousPreviousLayout = nullptr) override;
  bool moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout = nullptr, ExpressionLayout * previousLayout = nullptr, ExpressionLayout * previousPreviousLayout = nullptr) override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "binomial");
  }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  ExpressionLayout * kLayout();
  ExpressionLayout * nLayout();
};

}

#endif
