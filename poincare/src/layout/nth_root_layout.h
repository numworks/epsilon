#ifndef POINCARE_NTH_ROOT_LAYOUT_H
#define POINCARE_NTH_ROOT_LAYOUT_H

#include <poincare/bounded_static_layout_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class NthRootLayout : public BoundedStaticLayoutHierarchy<2> {
public:
  constexpr static KDCoordinate k_leftRadixHeight = 8;
  constexpr static KDCoordinate k_leftRadixWidth = 5;
  using BoundedStaticLayoutHierarchy::BoundedStaticLayoutHierarchy;
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
  bool hasUpperLeftIndex() const override { return numberOfChildren() > 1; }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_rightRadixHeight = 2;
  constexpr static KDCoordinate k_radixHorizontalOverflow = 2;
  constexpr static KDCoordinate k_indexHeight = 4;
  constexpr static KDCoordinate k_heightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 2;
  constexpr static KDCoordinate k_radixLineThickness = 1;
  KDSize adjustedIndexSize();
  ExpressionLayout * radicandLayout() { return editableChild(0); }
  ExpressionLayout * indexLayout() { return numberOfChildren() > 1 ? editableChild(1) : nullptr; }
};

}

#endif
