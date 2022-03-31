#ifndef CALCULATION_EXPRESSION_FIELD_H
#define CALCULATION_EXPRESSION_FIELD_H

#include <escher/expression_field.h>

namespace Calculation {

class ExpressionField : public Escher::ExpressionField {
public:
  ExpressionField(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandler, Escher::TextFieldDelegate * textFieldDelegate, Escher::LayoutFieldDelegate * layoutFieldDelegate) :
  Escher::ExpressionField(parentResponder, inputEventHandler, textFieldDelegate, layoutFieldDelegate) {
    setLayoutInsertionCursorEvent(Ion::Events::Up);
  }
protected:
  bool handleEvent(Ion::Events::Event event) override;
private:
  bool fieldContainsSingleMinusSymbol() const;
};

}

#endif
