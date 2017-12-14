#ifndef EXPRESSION_EDITOR_EXPRESSION_EDITOR_VIEW_H
#define EXPRESSION_EDITOR_EXPRESSION_EDITOR_VIEW_H

#include <escher.h>
#include "expression_and_layout.h"
#include "scrollable_expression_view_with_cursor.h"

namespace ExpressionEditor {

class ExpressionEditorView : public SolidColorView {
public:
  ExpressionEditorView(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, Poincare::ExpressionLayoutCursor * cursor);
  void cursorPositionChanged();
  int numberOfSubviews() const override { return 1; }
  ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor() { return &m_scrollableExpressionViewWithCursor; }
  View * subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_scrollableExpressionViewWithCursor;
  }
  void layoutSubviews() override;
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  constexpr static KDCoordinate k_margin = 10;
  ScrollableExpressionViewWithCursor m_scrollableExpressionViewWithCursor;
};

}

#endif
