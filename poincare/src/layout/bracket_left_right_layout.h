#ifndef POINCARE_BRACKET_LEFT_RIGHT_LAYOUT_H
#define POINCARE_BRACKET_LEFT_RIGHT_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class BracketLeftRightLayout : public ExpressionLayout {
public:
  BracketLeftRightLayout();
  ~BracketLeftRightLayout() {}
  BracketLeftRightLayout(const BracketLeftRightLayout& other) = delete;
  BracketLeftRightLayout(BracketLeftRightLayout&& other) = delete;
  BracketLeftRightLayout& operator=(const BracketLeftRightLayout& other) = delete;
  BracketLeftRightLayout& operator=(BracketLeftRightLayout&& other) = delete;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  constexpr static KDCoordinate k_bracketWidth = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_externWidthMargin = 2;
protected:
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override { return nullptr; }
  KDPoint positionOfChild(ExpressionLayout * child) override;
  uint16_t m_operandHeight;
};
}

#endif
