#ifndef POINCARE_PARENTHESIS_LAYOUT_H
#define POINCARE_PARENTHESIS_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class ParenthesisLayout : public ExpressionLayout {
public:
  ParenthesisLayout(ExpressionLayout * operandLayout);
  ~ParenthesisLayout();
  ParenthesisLayout(const ParenthesisLayout& other) = delete;
  ParenthesisLayout(ParenthesisLayout&& other) = delete;
  ParenthesisLayout& operator=(const ParenthesisLayout& other) = delete;
  ParenthesisLayout& operator=(ParenthesisLayout&& other) = delete;
  constexpr static KDCoordinate k_parenthesisCurveWidth = 5;
  constexpr static KDCoordinate k_parenthesisCurveHeight = 7;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_externWidthMargin = 1;
  constexpr static KDCoordinate k_externHeightMargin = 2;
  constexpr static KDCoordinate k_widthMargin = 5;
  constexpr static KDCoordinate k_lineThickness = 1;
  KDSize operandSize();
  ExpressionLayout * m_operandLayout;
};

}

#endif
