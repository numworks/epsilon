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
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveUp(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  bool moveDown(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout, ExpressionLayout * previousLayout, ExpressionLayout * previousPreviousLayout) override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  bool mustHaveLeftBrother() const override { return true; }
protected:
  ExpressionLayout * indiceLayout();
  ExpressionLayout * baseLayout();
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  Type m_type;
private:
  constexpr static KDCoordinate k_indiceHeight = 5;
};

}

#endif
