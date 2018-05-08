#ifndef ESCHER_EXPRESSION_LAYOUT_FIELD_DELEGATE_H
#define ESCHER_EXPRESSION_LAYOUT_FIELD_DELEGATE_H

#include <escher/toolbox.h>
#include <ion/events.h>

class ExpressionLayoutField;

class ExpressionLayoutFieldDelegate {
public:
  virtual bool expressionLayoutFieldShouldFinishEditing(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) = 0;
  virtual bool expressionLayoutFieldDidReceiveEvent(ExpressionLayoutField * expressionLayoutField, Ion::Events::Event event) = 0;
  virtual bool expressionLayoutFieldDidFinishEditing(ExpressionLayoutField * expressionLayoutField, const char * text, Ion::Events::Event event) { return false; }
  virtual bool expressionLayoutFieldDidAbortEditing(ExpressionLayoutField * expressionLayoutField) { return false; }
  virtual void expressionLayoutFieldDidChangeSize(ExpressionLayoutField * expressionLayoutField) {}
  virtual Toolbox * toolboxForExpressionLayoutField(ExpressionLayoutField * expressionLayoutField) = 0;
};

#endif
