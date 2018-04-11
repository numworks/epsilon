#ifndef ESCHER_SCROLLABLE_EXPRESSION_VIEW_WITH_CURSOR_H
#define ESCHER_SCROLLABLE_EXPRESSION_VIEW_WITH_CURSOR_H

#include <escher/scrollable_view.h>
#include <escher/expression_view_with_cursor.h>
#include <escher/scrollable_expression_view_with_cursor_delegate.h>
#include <poincare/expression_layout_cursor.h>

class ScrollableExpressionViewWithCursor : public ScrollableView, public ScrollViewDataSource {
public:
  ScrollableExpressionViewWithCursor(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, ScrollableExpressionViewWithCursorDelegate * delegate = nullptr);
  void setDelegate(ScrollableExpressionViewWithCursorDelegate * delegate) { m_delegate = delegate; }
  ExpressionViewWithCursor * expressionViewWithCursor() { return &m_expressionViewWithCursor; }
  bool isEditing() const;
  void setEditing(bool isEditing);
  void clearLayout();
  void scrollToCursor();
  void reload();

  /* Responder */
  Toolbox * toolbox() override;
  bool handleEvent(Ion::Events::Event event) override;

  bool scrollableExpressionViewWithCursorShouldFinishEditing(Ion::Events::Event event);

  void insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout);
  void insertLayoutFromTextAtCursor(const char * text);

  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;

protected:
  virtual bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);
  ExpressionViewWithCursor m_expressionViewWithCursor;
private:
  ScrollableExpressionViewWithCursorDelegate * m_delegate;
};

#endif
