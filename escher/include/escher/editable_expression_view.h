#ifndef ESCHER_EDITABLE_EXPRESSION_VIEW_H
#define ESCHER_EDITABLE_EXPRESSION_VIEW_H

#include <escher/scrollable_view.h>
#include <escher/expression_view_with_cursor.h>
#include <escher/editable_expression_view_delegate.h>
#include <poincare/expression_layout_cursor.h>

class EditableExpressionView : public ScrollableView, public ScrollViewDataSource {
public:
  EditableExpressionView(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, EditableExpressionViewDelegate * delegate = nullptr);
  void setDelegate(EditableExpressionViewDelegate * delegate) { m_delegate = delegate; }
  ExpressionViewWithCursor * expressionViewWithCursor() { return &m_expressionViewWithCursor; }
  bool isEditing() const;
  void setEditing(bool isEditing);
  void scrollToCursor();
  void reload();

  /* Responder */
  Toolbox * toolbox() override;
  bool handleEvent(Ion::Events::Event event) override;

  bool editableExpressionViewShouldFinishEditing(Ion::Events::Event event);

  void insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout);
  void insertLayoutFromTextAtCursor(const char * text);

  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;

protected:
  virtual bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);
  ExpressionViewWithCursor m_expressionViewWithCursor;
private:
  EditableExpressionViewDelegate * m_delegate;
};

#endif
