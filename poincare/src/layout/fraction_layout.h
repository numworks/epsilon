#ifndef POINCARE_FRACTION_LAYOUT_H
#define POINCARE_FRACTION_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class FractionLayout : public StaticLayoutHierarchy<2> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;

  // Collapse
  void collapseSiblingsAndMoveCursor(ExpressionLayoutCursor * cursor) override;

  // User input
  void deleteBeforeCursor(ExpressionLayoutCursor * cursor) override;

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  ExpressionLayoutCursor cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // Serialization
  int writeTextInBuffer(char * buffer, int bufferSize) const override;

  // Other
  ExpressionLayout * layoutToPointWhenInserting() override;
  bool canBeOmittedMultiplicationRightFactor() const override { return false; }
  /* WARNING: We need to override this function, else 1/2 3/4 would be
   * serialized as 1/2**3/4, as the two Fraction layouts think their sibling is
   * an omitted multiplication layout factor. We have the same problem with
   *  2^3 1/2 being serialized as 2^3**1/2, so must override the Right version
   * and not canBeOmittedMultiplicationLeftFactor. */
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_fractionLineMargin = 2;
  constexpr static KDCoordinate k_fractionLineHeight = 1;
  ExpressionLayout * numeratorLayout() { return editableChild(0); }
  ExpressionLayout * denominatorLayout() { return editableChild(1); }
};

}

#endif
