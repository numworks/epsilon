#ifndef CALCULATION_LAYOUT_FIELD_H
#define CALCULATION_LAYOUT_FIELD_H

#include <escher/expression_input_bar.h>
#include <poincare/old/trinary_boolean.h>

namespace Calculation {

class LayoutField : public Escher::LayoutField {
 public:
  LayoutField(Escher::Responder* parentResponder,
              Escher::LayoutFieldDelegate* layoutFieldDelegate)
      : Escher::LayoutField(parentResponder, layoutFieldDelegate),
        m_insertionCursor(),
        m_currentStep(DivisionCycleStep::Start),
        m_divisionCycleWithAns(Poincare::TrinaryBoolean::Unknown) {}

  void updateCursorBeforeInsertion();

 protected:
  bool handleEvent(Ion::Events::Event event) override;

 private:
  enum class DivisionCycleStep : uint8_t {
    Start = 0,
    DenominatorOfAnsFraction,    // cursor at Denominator of Ans/Empty
    DenominatorOfEmptyFraction,  // cursor at Denominator of Empty/Empty
    NumeratorOfEmptyFraction,    // cursor at Numerator of Empty/Empty
    MixedFraction,               // cursor before Empty/Empty
  };

  void resetInsertionCursor() {
    m_insertionCursor = Poincare::Internal::LayoutBufferCursor();
  }
  bool handleDivision();

  Poincare::Internal::LayoutBufferCursor m_insertionCursor;
  DivisionCycleStep m_currentStep;
  Poincare::TrinaryBoolean m_divisionCycleWithAns;
};

using ExpressionInputBar = Escher::TemplatedExpressionInputBar<LayoutField>;

}  // namespace Calculation

#endif
