#ifndef CALCULATION_SCROLLABLE_EXPRESSION_VIEW_H
#define CALCULATION_SCROLLABLE_EXPRESSION_VIEW_H

#include <escher.h>

namespace Calculation {

class ScrollableExpressionView : public ScrollView, public Responder {
public:
  ScrollableExpressionView(Responder * parentResponder);
  void setExpression(Poincare::ExpressionLayout * expressionLayout);
  void layoutSubviews() override;
  void setBackgroundColor(KDColor backgroundColor);
  bool handleEvent(Ion::Events::Event event) override;
  KDSize minimalSizeForOptimalDisplay() override;
  void reloadCell();
private:
	bool rightViewIsInvisible();
  ExpressionView m_expressionView;
  KDCoordinate m_manualScrolling;
};

}

#endif
