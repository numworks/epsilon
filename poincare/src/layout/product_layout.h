#ifndef POINCARE_PRODUCT_LAYOUT_H
#define POINCARE_PRODUCT_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class ProductLayout : public ExpressionLayout {
public:
  ProductLayout(ExpressionLayout * lowerBoundLayout, ExpressionLayout * upperBoundLayout, ExpressionLayout * argumentLayout);
  ~ProductLayout();
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_symbolHeight = 15;
  constexpr static KDCoordinate k_symbolWidth = 9;
  constexpr static KDCoordinate k_boundHeightMargin = 2;
  constexpr static KDCoordinate k_argumentWidthMargin = 2;
  constexpr static KDCoordinate k_lineThickness = 1;
  ExpressionLayout * m_lowerBoundLayout;
  ExpressionLayout * m_upperBoundLayout;
  ExpressionLayout * m_argumentLayout;
};

#endif
