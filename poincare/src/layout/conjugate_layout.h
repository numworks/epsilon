#ifndef POINCARE_CONJUGATE_LAYOUT_H
#define POINCARE_CONJUGATE_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class ConjugateLayout : public ExpressionLayout {
public:
  ConjugateLayout(ExpressionLayout * operandLayout);
  ~ConjugateLayout();
  ConjugateLayout(const ConjugateLayout& other) = delete;
  ConjugateLayout(ConjugateLayout&& other) = delete;
  ConjugateLayout& operator=(const ConjugateLayout& other) = delete;
  ConjugateLayout& operator=(ConjugateLayout&& other) = delete;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  constexpr static KDCoordinate k_overlineWidth = 1;
  constexpr static KDCoordinate k_overlineMargin = 3;
  ExpressionLayout * m_operandLayout;
};

}

#endif

