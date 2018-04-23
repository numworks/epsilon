#ifndef POINCARE_SEQUENCE_LAYOUT_H
#define POINCARE_SEQUENCE_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <assert.h>

namespace Poincare {

class SequenceLayout : public StaticLayoutHierarchy<3> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  constexpr static KDCoordinate k_symbolHeight = 15;
  constexpr static KDCoordinate k_symbolWidth = 9;

  // User input
  void deleteBeforeCursor(ExpressionLayoutCursor * cursor) override;

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  ExpressionLayoutCursor cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // Other
  ExpressionLayout * layoutToPointWhenInserting() override {
    assert(lowerBoundLayout() != nullptr);
    return lowerBoundLayout();
  }
  char XNTChar() const override { return 'n'; }
protected:
  constexpr static KDCoordinate k_boundHeightMargin = 2;
  constexpr static KDCoordinate k_argumentWidthMargin = 2;
  int writeDerivedClassInBuffer(const char * operatorName, char * buffer, int bufferSize) const;
  KDSize computeSize() override;
  KDPoint positionOfChild(ExpressionLayout * eL) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  ExpressionLayout * upperBoundLayout() { return editableChild(2); }
  ExpressionLayout * lowerBoundLayout() { return editableChild(1); }
  ExpressionLayout * argumentLayout() { return editableChild(0); }
private:
  void computeBaseline() override;
};

}

#endif
