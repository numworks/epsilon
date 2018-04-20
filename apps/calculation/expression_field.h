#ifndef CALCULATION_EXPRESSION_FIELD_H
#define CALCULATION_EXPRESSION_FIELD_H

#include <escher.h>

namespace Calculation {

class ExpressionField : public ::ExpressionField {
public:
  ExpressionField(Responder * parentResponder, char * textBuffer, int textBufferLength, Poincare::ExpressionLayout * layout, TextFieldDelegate * textFieldDelegate, ExpressionLayoutFieldDelegate * expressionLayoutFieldDelegate);
protected:
  bool handleEvent(Ion::Events::Event event) override;
};

}

#endif
