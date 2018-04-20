#ifndef CALCULATION_EDITABLE_EXPRESSION_VIEW_H
#define CALCULATION_EDITABLE_EXPRESSION_VIEW_H

#include <escher.h>

namespace Calculation {

class EditableExpressionView : public ::EditableExpressionView {
public:
  EditableExpressionView(Responder * parentResponder, TextFieldDelegate * textFieldDelegate, ExpressionLayoutFieldDelegate * expressionLayoutFieldDelegate);
protected:
  bool handleEvent(Ion::Events::Event event) override;
};

}

#endif
