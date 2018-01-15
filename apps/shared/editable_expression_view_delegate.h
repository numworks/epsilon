#ifndef SHARED_EDITABLE_EXPRESSION_VIEW_DELEGATE_H
#define SHARED_EDITABLE_EXPRESSION_VIEW_DELEGATE_H

#include <escher/editable_expression_view_delegate.h>
#include "text_field_and_editable_expression_view_delegate_app.h"

namespace Shared {

class EditableExpressionViewDelegate : public ::EditableExpressionViewDelegate {
public:
  bool editableExpressionViewShouldFinishEditing(EditableExpressionView * editableExpressionView, Ion::Events::Event event) override;
  bool editableExpressionViewDidReceiveEvent(EditableExpressionView * editableExpressionView, Ion::Events::Event event) override;
  Toolbox * toolboxForEditableExpressionView(EditableExpressionView * editableExpressionView) override;
private:
  virtual TextFieldAndEditableExpressionViewDelegateApp * textFieldAndEditableExpressionViewDelegateApp() = 0;
};

}

#endif
