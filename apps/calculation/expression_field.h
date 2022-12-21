#ifndef CALCULATION_EXPRESSION_FIELD_H
#define CALCULATION_EXPRESSION_FIELD_H

#include <escher/expression_field.h>
#include <poincare/trinary_boolean.h>

namespace Calculation {

class ExpressionField : public Escher::ExpressionField {
public:
  ExpressionField(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandler, Escher::TextFieldDelegate * textFieldDelegate, Escher::LayoutFieldDelegate * layoutFieldDelegate) :
  Escher::ExpressionField(parentResponder, inputEventHandler, textFieldDelegate, layoutFieldDelegate),
  m_currentStep(DivisionCycleStep::Start),
  m_divisionCycleWithAns(Poincare::TrinaryBoolean::Unknown) {
    setLayoutInsertionCursorEvent(Ion::Events::Up);
  }
protected:
  bool handleEvent(Ion::Events::Event event) override;
private:
  enum class DivisionCycleStep : uint8_t {
    Start = 0,
    DenominatorOfAnsFraction, // cursor at Denominator of Ans/Empty
    DenominatorOfEmptyFraction, // cursor at Denominator of Empty/Empty
    NumeratorOfEmptyFraction, // cursor at Numerator of Empty/Empty
    MixedFraction, // cursor before Empty/Empty
  };

  bool fieldContainsSingleMinusSymbol() const;
  bool handleDivision();

  DivisionCycleStep m_currentStep;
  Poincare::TrinaryBoolean m_divisionCycleWithAns;
};

}

#endif
