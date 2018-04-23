#ifndef SHARED_EXPRESSION_FIELD_DELEGATE_APP_H
#define SHARED_EXPRESSION_FIELD_DELEGATE_APP_H

#include "text_field_delegate_app.h"
#include <escher/expression_layout_field_delegate.h>

namespace Shared {

class ExpressionFieldDelegateApp : public TextFieldDelegateApp, public ExpressionLayoutFieldDelegate {
public:
  virtual ~ExpressionFieldDelegateApp() = default;
  bool expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  virtual bool expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  Toolbox * toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) override;
protected:
  char privateXNT(ExpressionLayoutField * expressionLayoutField);
  ExpressionFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController);
};

}

#endif
