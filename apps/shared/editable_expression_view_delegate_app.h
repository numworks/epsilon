#ifndef SHARED_EDITABLE_EXPRESSION_VIEW_DELEGATE_APP_H
#define SHARED_EDITABLE_EXPRESSION_VIEW_DELEGATE_APP_H

#include "text_field_delegate_app.h"
#include <escher/scrollable_expression_view_with_cursor_delegate.h>

namespace Shared {

class EditableExpressionViewDelegateApp : public TextFieldDelegateApp, public ScrollableExpressionViewWithCursorDelegate {
public:
  virtual ~EditableExpressionViewDelegateApp() = default;
  bool scrollableExpressionViewWithCursorShouldFinishEditing(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) override;
  virtual bool scrollableExpressionViewWithCursorDidReceiveEvent(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor, Ion::Events::Event event) override;
  Toolbox * toolboxForScrollableExpressionViewWithCursor(ScrollableExpressionViewWithCursor * scrollableExpressionViewWithCursor) override;
protected:
  EditableExpressionViewDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
};

}

#endif
