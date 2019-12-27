#ifndef ESCHER_SCROLLABLE_EXPRESSION_VIEW_H
#define ESCHER_SCROLLABLE_EXPRESSION_VIEW_H

#include <escher/scrollable_view.h>
#include <escher/scroll_view_data_source.h>
#include <escher/expression_view.h>

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

#endif
