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

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  ExpressionLayoutCursor cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // Serialization
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "binomial");
  }

protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  ExpressionLayout * nLayout() { return editableChild(0); }
  ExpressionLayout * kLayout() { return editableChild(1); }
};

}

#endif
