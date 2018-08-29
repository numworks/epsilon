#ifndef POINCARE_INTEGRAL_LAYOUT_H
#define POINCARE_INTEGRAL_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/layout_engine.h>
#include <assert.h>

namespace Poincare {

class IntegralLayout : public StaticLayoutHierarchy<3> {
public:
  constexpr static KDCoordinate k_symbolHeight = 4;
  constexpr static KDCoordinate k_symbolWidth = 4;
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;

  // Dynamic Layout
  void deleteBeforeCursor(ExpressionLayoutCursor * cursor) override;

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  ExpressionLayoutCursor cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // Serialization
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;

  // Other
  ExpressionLayout * layoutToPointWhenInserting() override {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout();
  }
  char XNTChar() const override { return 'x'; }
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
  ExpressionLayout * integrandLayout() { return editableChild(0); }
  ExpressionLayout * lowerBoundLayout() { return editableChild(1); }
  ExpressionLayout * upperBoundLayout() { return editableChild(2); }
};

}

#endif
