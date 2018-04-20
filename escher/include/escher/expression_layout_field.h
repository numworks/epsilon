#ifndef ESCHER_EXPRESSION_LAYOUT_FIELD_H
#define ESCHER_EXPRESSION_LAYOUT_FIELD_H

#include <escher/scrollable_view.h>
#include <escher/expression_view_with_cursor.h>
#include <escher/expression_layout_field_delegate.h>
#include <poincare/expression_layout_cursor.h>

class ExpressionLayoutField : public ScrollableView, public ScrollViewDataSource {
public:
  ExpressionLayoutField(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout, ExpressionLayoutFieldDelegate * delegate = nullptr);
  void setDelegate(ExpressionLayoutFieldDelegate * delegate) { m_delegate = delegate; }
  ExpressionViewWithCursor * expressionViewWithCursor() { return &m_expressionViewWithCursor; }
  bool isEditing() const;
  void setEditing(bool isEditing);
  void clearLayout();
  void scrollToCursor();
  void reload();

  /* Responder */
  Toolbox * toolbox() override;
  bool handleEvent(Ion::Events::Event event) override;

  bool expressionLayoutFieldShouldFinishEditing(Ion::Events::Event event);

  void insertLayoutAtCursor(Poincare::ExpressionLayout * layout, Poincare::ExpressionLayout * pointedLayout);
  void insertLayoutFromTextAtCursor(const char * text);

  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;

protected:
  virtual bool privateHandleEvent(Ion::Events::Event event);
  bool privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout);
  ExpressionViewWithCursor m_expressionViewWithCursor;
private:
  ExpressionLayoutFieldDelegate * m_delegate;
};

#endif
