#ifndef CALCULATION_SCROLLABLE_EXPRESSION_VIEW_H
#define CALCULATION_SCROLLABLE_EXPRESSION_VIEW_H

#include <escher.h>

namespace Calculation {

class ScrollableExpressionView final : public ScrollableView, public ScrollViewDataSource {
public:
  ScrollableExpressionView(Responder * parentResponder) :
    ScrollableView(parentResponder, &m_expressionView, this),
    m_expressionView() {}
  void setExpressionLayout(Poincare::ExpressionLayout * expressionLayout) {
    m_expressionView.setExpressionLayout(expressionLayout);
    layoutSubviews();
  }
  void setBackgroundColor(KDColor backgroundColor) override {
    m_expressionView.setBackgroundColor(backgroundColor);
    ScrollableView::setBackgroundColor(backgroundColor);
  }
  KDSize minimalSizeForOptimalDisplay() const override {
    return m_expressionView.minimalSizeForOptimalDisplay();
  }
private:
  ExpressionView m_expressionView;
};

}

#endif
