#ifndef EXPRESSION_EDITOR_SCROLLABLE_EXPRESSION_VIEW_WITH_CURSOR_H
#define EXPRESSION_EDITOR_SCROLLABLE_EXPRESSION_VIEW_WITH_CURSOR_H

#include <escher.h>
#include <poincare/expression_layout.h>
#include <poincare/expression_layout_cursor.h>
#include "expression_view_with_cursor.h"

namespace ExpressionEditor {

class ScrollableExpressionViewWithCursor : public ScrollableView, public ScrollViewDataSource {
public:
  ScrollableExpressionViewWithCursor(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, Poincare::ExpressionLayoutCursor * cursor);
  KDSize minimalSizeForOptimalDisplay() const override;
  ExpressionViewWithCursor * expressionViewWithCursor() { return &m_expressionViewWithCursor; }
  void scrollToCursor();
  /* ScrollableView */
  bool handleEvent(Ion::Events::Event event) override { return false; }
private:
  ExpressionViewWithCursor m_expressionViewWithCursor;
};

}

#endif
