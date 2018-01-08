#ifndef POINCARE_PARENTHESIS_LAYOUT_H
#define POINCARE_PARENTHESIS_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/layout_engine.h>

/* ParenthesisLayout has one operand and renders the parentheses around it.
 * This layout should thus be used for uneditable parentheses, e.g. to create
 * binomial coefficient layouts. */

namespace Poincare {

class ParenthesisLayout : public StaticLayoutHierarchy<1> {
public:
  using StaticLayoutHierarchy::StaticLayoutHierarchy;
  ExpressionLayout * clone() const override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionLayoutTextInBuffer(this, buffer, bufferSize, "");
  }
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_externWidthMargin = 1;
  constexpr static KDCoordinate k_externHeightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  ExpressionLayout * operandLayout();
};

}

#endif
