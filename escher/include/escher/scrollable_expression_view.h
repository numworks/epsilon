#ifndef ESCHER_SCROLLABLE_EXPRESSION_VIEW_H
#define ESCHER_SCROLLABLE_EXPRESSION_VIEW_H

#include <escher/scrollable_view.h>
#include <escher/scroll_view_data_source.h>
#include <escher/expression_view.h>

class ScrollableExpressionView : public ScrollableView, public ScrollViewDataSource {
public:
  ScrollableExpressionView(Responder * parentResponder, KDCoordinate leftRightMargin, KDCoordinate topBottomMargin, float horizontalAlignment = 0.0f, float verticalAlignment = 0.5f, KDColor textColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  Poincare::Layout layout() const;
  void setLayout(Poincare::Layout layout);
  void setBackgroundColor(KDColor backgroundColor) override;
  void setExpressionBackgroundColor(KDColor backgroundColor);
private:
  ExpressionView m_expressionView;
};

#endif
