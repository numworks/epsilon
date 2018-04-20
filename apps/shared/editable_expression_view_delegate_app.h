#ifndef SHARED_EDITABLE_EXPRESSION_VIEW_DELEGATE_APP_H
#define SHARED_EDITABLE_EXPRESSION_VIEW_DELEGATE_APP_H

#include "text_field_delegate_app.h"
#include <escher/expression_layout_field_delegate.h>

namespace Shared {

class EditableExpressionViewDelegateApp : public TextFieldDelegateApp, public ExpressionLayoutFieldDelegate {
public:
  virtual ~EditableExpressionViewDelegateApp() = default;
  bool expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  virtual bool expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  Toolbox * toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) override;
protected:
  EditableExpressionViewDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
};

}

#endif
