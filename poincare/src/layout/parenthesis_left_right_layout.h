#ifndef POINCARE_PARENTHESIS_LEFT_RIGHT_LAYOUT_H
#define POINCARE_PARENTHESIS_LEFT_RIGHT_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class ParenthesisLeftRightLayout : public StaticLayoutHierarchy<0> {
public:
  ParenthesisLeftRightLayout();
  void invalidAllSizesPositionsAndBaselines() override;
  bool moveLeft(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  bool moveRight(ExpressionLayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  constexpr static KDCoordinate parenthesisWidth() { return k_widthMargin + k_lineThickness + k_externWidthMargin; }
  constexpr static KDCoordinate k_parenthesisCurveWidth = 5;
  constexpr static KDCoordinate k_parenthesisCurveHeight = 7;
  constexpr static KDCoordinate k_externWidthMargin = 1;
  constexpr static KDCoordinate k_externHeightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_verticalMargin = 4;
protected:
  KDColor s_parenthesisWorkingBuffer[k_parenthesisCurveHeight*k_parenthesisCurveWidth];
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
