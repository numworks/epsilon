#ifndef POINCARE_SUBSCRIPT_LAYOUT_H
#define POINCARE_SUBSCRIPT_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

class SubscriptLayout : public ExpressionLayout {
  public:
    SubscriptLayout(ExpressionLayout * baseLayout, ExpressionLayout * subscriptLayout);
    ~SubscriptLayout();
  protected:
    void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
    KDSize computeSize() override;
    ExpressionLayout * child(uint16_t index) override;
    KDPoint positionOfChild(ExpressionLayout * child) override;
  private:
    constexpr static KDCoordinate k_subscriptHeight = 5;
    ExpressionLayout * m_baseLayout;
    ExpressionLayout * m_subscriptLayout;
};

#endif
