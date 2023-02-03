#ifndef CALCULATION_EXPRESSION_INPUT_BAR_H
#define CALCULATION_EXPRESSION_INPUT_BAR_H

#include <escher/expression_input_bar.h>
#include <poincare/trinary_boolean.h>

namespace Calculation {

class ExpressionInputBar : public Escher::ExpressionInputBar {
public:
  ExpressionInputBar(Escher::Responder * parentResponder, Escher::InputEventHandlerDelegate * inputEventHandler, Escher::LayoutFieldDelegate * layoutFieldDelegate) :
  Escher::ExpressionInputBar(parentResponder, inputEventHandler, layoutFieldDelegate),
  m_currentStep(DivisionCycleStep::Start),
  m_divisionCycleWithAns(Poincare::TrinaryBoolean::Unknown) {}
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
