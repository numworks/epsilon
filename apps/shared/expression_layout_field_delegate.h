#ifndef SHARED_EXPRESSION_LAYOUT_FIELD_DELEGATE_H
#define SHARED_EXPRESSION_LAYOUT_FIELD_DELEGATE_H

#include <escher/expression_layout_field_delegate.h>
#include "expression_field_delegate_app.h"

namespace Shared {

class ExpressionLayoutFieldDelegate : public ::ExpressionLayoutFieldDelegate {
public:
  bool expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidFinishEditing(ExpressionLayoutField * expressionLayoutField, const char * text, Ion::Events::Event event) override;
  bool expressionLayoutFieldDidAbortEditing(ExpressionLayoutField * expressionLayoutField) override;
  void expressionLayoutFieldDidChangeSize(ExpressionLayoutField * expressionLayoutField) override;
  Toolbox * toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) override;
private:
  virtual ExpressionFieldDelegateApp * expressionFieldDelegateApp() = 0;
};

}

#endif
