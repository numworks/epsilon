#ifndef CALCULATION_SCROLLABLE_EXPRESSION_VIEW_H
#define CALCULATION_SCROLLABLE_EXPRESSION_VIEW_H

#include <escher.h>

namespace Calculation {

class ScrollableExpressionView : public ScrollableView, public ScrollViewDataSource {
public:
  ScrollableExpressionView(Responder * parentResponder);
  Poincare::Layout layout() const;
  void setLayout(Poincare::Layout layout);
  void setBackgroundColor(KDColor backgroundColor) override;
  void setExpressionBackgroundColor(KDColor backgroundColor);
private:
  ExpressionView m_expressionView;
};

}

#endif
