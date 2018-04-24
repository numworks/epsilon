#ifndef POINCARE_SQUARE_BRACKET_LAYOUT_H
#define POINCARE_SQUARE_BRACKET_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class SquareBracketLayout : public StaticLayoutHierarchy<0> {
public:
  SquareBracketLayout();
  void invalidAllSizesPositionsAndBaselines() override;
  ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
protected:
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_externWidthMargin = 2;
  KDSize computeSize() override;
  void computeBaseline() override;
  KDCoordinate operandHeight();
  void computeOperandHeight();
  KDPoint positionOfChild(ExpressionLayout * child) override;
  bool m_operandHeightComputed;
  uint16_t m_operandHeight;
};
}

#endif
