#ifndef POINCARE_BRACKET_LEFT_RIGHT_LAYOUT_H
#define POINCARE_BRACKET_LEFT_RIGHT_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>

namespace Poincare {

class BracketLeftRightLayout : public StaticLayoutHierarchy<0> {
public:
  BracketLeftRightLayout();
  void invalidAllSizesPositionsAndBaselines() override;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_externWidthMargin = 2;
protected:
  constexpr static KDCoordinate k_minimalOperandHeight = 18;
  KDSize computeSize() override;
  KDCoordinate operandHeight();
  virtual void computeOperandHeight() = 0;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  bool m_operandHeightComputed;
  uint16_t m_operandHeight;
};
}

#endif
