#ifndef SHARED_TEXT_FIELD_AND_EDITABLE_EXPRESSION_VIEW_DELEGATE_APP_H
#define SHARED_TEXT_FIELD_AND_EDITABLE_EXPRESSION_VIEW_DELEGATE_APP_H

#include "text_field_delegate_app.h"
#include <escher/editable_expression_view_delegate.h>

namespace Shared {

class TextFieldAndEditableExpressionViewDelegateApp : public TextFieldDelegateApp, public EditableExpressionViewDelegate {
public:
  virtual ~TextFieldAndEditableExpressionViewDelegateApp() = default;
  bool editableExpressionViewShouldFinishEditing(EditableExpressionView * editableExpressionView, Ion::Events::Event event) override;
  virtual bool editableExpressionViewDidReceiveEvent(EditableExpressionView * editableExpressionView, Ion::Events::Event event) override;
  Toolbox * toolboxForEditableExpressionView(EditableExpressionView * editableExpressionView) override;
protected:
  TextFieldAndEditableExpressionViewDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
};

}

#endif
