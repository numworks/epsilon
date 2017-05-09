#ifndef POINCARE_INTEGRAL_LAYOUT_H
#define POINCARE_INTEGRAL_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class IntegralLayout : public ExpressionLayout {
public:
  IntegralLayout(ExpressionLayout * lowerBoundLayout, ExpressionLayout * upperBoundLayout, ExpressionLayout * integrandLayout);
  ~IntegralLayout();
  IntegralLayout(const IntegralLayout& other) = delete;
  IntegralLayout(IntegralLayout&& other) = delete;
  IntegralLayout& operator=(const IntegralLayout& other) = delete;
  IntegralLayout& operator=(IntegralLayout&& other) = delete;
  constexpr static KDCoordinate k_symbolHeight = 4;
  constexpr static KDCoordinate k_symbolWidth = 4;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_boundHeightMargin = 8;
  constexpr static KDCoordinate k_boundWidthMargin = 5;
  constexpr static KDCoordinate k_integrandWidthMargin = 2;
  constexpr static KDCoordinate k_integrandHeigthMargin = 2;
  constexpr static KDCoordinate k_lineThickness = 1;
  ExpressionLayout * m_lowerBoundLayout;
  ExpressionLayout * m_upperBoundLayout;
  ExpressionLayout * m_integrandLayout;
};

}

#endif
