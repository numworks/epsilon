#ifndef POINCARE_CONDENSED_SUM_LAYOUT_H
#define POINCARE_CONDENSED_SUM_LAYOUT_H

#include <poincare/expression.h>
#include <poincare/expression_layout.h>

namespace Poincare {

class CondensedSumLayout : public ExpressionLayout {
public:
  CondensedSumLayout(ExpressionLayout * baseLayout, ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout = nullptr);
  ~CondensedSumLayout();
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  ExpressionLayout * m_baseLayout;
  ExpressionLayout * m_subscriptLayout;
  ExpressionLayout * m_superscriptLayout;
};

}

#endif

