#ifndef CALCULATION_SCROLLABLE_EXPRESSION_VIEW_H
#define CALCULATION_SCROLLABLE_EXPRESSION_VIEW_H

#include <escher.h>

namespace Calculation {

class ScrollableExpressionView : public ScrollableView, public ScrollViewDataSource {
public:
  ScrollableExpressionView(Responder * parentResponder);
  void setExpressionLayout(Poincare::ExpressionLayout * expressionLayout);
  void setBackgroundColor(KDColor backgroundColor) override;
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  ExpressionView m_expressionView;
};

}

#endif
