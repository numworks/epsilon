#ifndef POINCARE_FRACTION_LAYOUT_H
#define POINCARE_FRACTION_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class FractionLayout : public ExpressionLayout {
public:
  FractionLayout(ExpressionLayout * numerator, ExpressionLayout * denominator);
  ~FractionLayout();
  FractionLayout(const FractionLayout& other) = delete;
  FractionLayout(FractionLayout&& other) = delete;
  FractionLayout& operator=(const FractionLayout& other) = delete;
  FractionLayout& operator=(FractionLayout&& other) = delete;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_fractionBorderLength = 2;
  constexpr static KDCoordinate k_fractionBorderMargin = 2;
  constexpr static KDCoordinate k_fractionLineMargin = 2;
  constexpr static KDCoordinate k_fractionLineHeight = 2;
  ExpressionLayout * m_numerator_layout;
  ExpressionLayout * m_denominator_layout;
};

}

#endif
