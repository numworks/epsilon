#ifndef CALCULATION_EXPRESSION_FIELD_H
#define CALCULATION_EXPRESSION_FIELD_H

#include <escher.h>

namespace Calculation {

class ExpressionField : public ::ExpressionField {
public:
  ExpressionField(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  ::ExpressionField(parentResponder, inputEventHandler, textFieldDelegate, layoutFieldDelegate) {
    setLayoutInsertionCursorEvent(Ion::Events::Up);
  }
protected:
  bool handleEvent(Ion::Events::Event event) override;
};

}

#endif
