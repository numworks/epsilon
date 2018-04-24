#ifndef POINCARE_BRACKET_LAYOUT_H
#define POINCARE_BRACKET_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class BracketLayout : public StaticLayoutHierarchy<0> {
public:
  BracketLayout();
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
