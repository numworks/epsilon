#ifndef POINCARE_PARENTHESIS_LEFT_RIGHT_LAYOUT_H
#define POINCARE_PARENTHESIS_LEFT_RIGHT_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class ParenthesisLeftRightLayout : public ExpressionLayout {
public:
  ParenthesisLeftRightLayout();
  ~ParenthesisLeftRightLayout() {}
  ParenthesisLeftRightLayout(const ParenthesisLeftRightLayout& other) = delete;
  ParenthesisLeftRightLayout(ParenthesisLeftRightLayout&& other) = delete;
  ParenthesisLeftRightLayout& operator=(const ParenthesisLeftRightLayout& other) = delete;
  ParenthesisLeftRightLayout& operator=(ParenthesisLeftRightLayout&& other) = delete;
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  constexpr static KDCoordinate k_parenthesisCurveWidth = 5;
  constexpr static KDCoordinate k_parenthesisCurveHeight = 7;
  constexpr static KDCoordinate k_externWidthMargin = 1;
  constexpr static KDCoordinate k_externHeightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
protected:
  KDColor s_parenthesisWorkingBuffer[k_parenthesisCurveHeight*k_parenthesisCurveWidth];
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override { return nullptr; }
  KDPoint positionOfChild(ExpressionLayout * child) override;
  uint16_t m_operandHeight;
};
}

#endif
