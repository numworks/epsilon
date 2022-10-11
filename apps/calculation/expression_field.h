#ifndef CALCULATION_EXPRESSION_FIELD_H
#define CALCULATION_EXPRESSION_FIELD_H

#include <escher/expression_field.h>

namespace Calculation {

class ExpressionField : public Escher::ExpressionField {
public:
  ExpressionField(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandler, Escher::TextFieldDelegate * textFieldDelegate, Escher::LayoutFieldDelegate * layoutFieldDelegate) :
  Escher::ExpressionField(parentResponder, inputEventHandler, textFieldDelegate, layoutFieldDelegate) {
    setLayoutInsertionCursorEvent(Ion::Events::Up);
    m_currentStep = DivisionCycleStep::Start;
    m_divisionStepUpToDate = false;
  }
protected:
  bool handleEvent(Ion::Events::Event event) override;
private:
  enum class DivisionCycleStep : uint8_t {
    Start = 0,
    DenominatorOfAnsFraction, // cursor at Denominator of Ans/Empty
    NumeratorOfEmptyFraction, // cursor at Numerator of Empty/Empty
    MixedFraction, // cursor before Empty/Empty
    NotCycling
  };

  DivisionCycleStep m_currentStep;
  bool m_divisionStepUpToDate;

  bool fieldContainsSingleMinusSymbol() const;
  bool handleDivision();
};

}

#endif
