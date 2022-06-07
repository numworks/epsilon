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
  enum class DivisionCycleStep : uint8_t {
    Start = 0,
    AnsDivided,
    EmptyFraction,
    MixedFraction,
    NotCycling
  };

  constexpr static const char * k_1DMixedFractionCommand = " /";
  constexpr static const char * k_1DAnsFraction = "Ans/";
  constexpr static const char * k_serializedEmptyFraction = "\U00000012\U00000012\U00000013/\U00000012\U00000013\U00000013";
  // This is a table to be able to compute its size.
  constexpr static const char k_serializedAnsFraction[] = "\U00000012\U00000012Ans\U00000013/\U00000012\U00000013\U00000013";
  constexpr static int k_divisionCycleCheckBufferSize = sizeof(k_serializedAnsFraction) + 1;

  bool fieldContainsSingleMinusSymbol() const;
  DivisionCycleStep currentStepOfDivisionCycling();
};

}

#endif
