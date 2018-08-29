#ifndef POINCARE_VERTICAL_OFFSET_LAYOUT_H
#define POINCARE_VERTICAL_OFFSET_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class VerticalOffsetLayout : public StaticLayoutHierarchy<1> {
public:
  enum class Type {
    Subscript,
    Superscript
  };
  VerticalOffsetLayout(ExpressionLayout * indice, Type type, bool cloneOperands);
  ExpressionLayout * clone() const override;

  // VerticalOffsetLayout
  Type type() const { return m_type; }

  // User input
  void deleteBeforeCursor(ExpressionLayoutCursor * cursor) override;

  // Tree navigation
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  ExpressionLayoutCursor cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // Serialization
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;

  // Other
  bool mustHaveLeftSibling() const override { return true; }
  bool isVerticalOffset() const override { return true; }
protected:
  ExpressionLayout * indiceLayout() { return editableChild(0);}
  ExpressionLayout * baseLayout();
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  void privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor) override;
  Type m_type;
private:
  constexpr static KDCoordinate k_indiceHeight = 5;
  constexpr static KDCoordinate k_separationMargin = 5;
};

}

#endif
