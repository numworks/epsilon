#pragma once

#include <escher/expression_input_bar.h>
#include <omg/troolean.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/symbol_context.h>
#include <poincare/user_expression.h>

namespace Calculation {

class LayoutField : public Escher::LayoutField {
 public:
  LayoutField(Escher::Responder* parentResponder,
              Escher::LayoutFieldDelegate* layoutFieldDelegate)
      : Escher::LayoutField(parentResponder, layoutFieldDelegate),
        m_insertionCursor(),
        m_currentStep(DivisionCycleStep::Start),
        m_divisionCycleWithAns(OMG::Troolean::Unknown) {}

  void updateCursorBeforeInsertion();

  static Poincare::UserExpression UserExpressionFromEditionField(
      Poincare::Layout layout) {
    Poincare::UserExpression currentExpression =
        Poincare::UserExpression::Parse(layout, Poincare::EmptySymbolContext{});
    return currentExpression;
  }

  bool containsTrigFunction() const {
    // Try to parse the expression
    Poincare::UserExpression userExpression =
        UserExpressionFromEditionField(layout());
    if (userExpression.isUninitialized()) {
      // Failed to parse the expression being edited, return false by default
      return false;
    }
    return userExpression.recursivelyMatches(
        [](const Poincare::UserExpression e) {
          return e.isTrigonometryFunction();
        });
  }

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

  void resetInsertionCursor() { m_insertionCursor = Poincare::LayoutCursor(); }
  bool handleDivision();

  Poincare::LayoutCursor m_insertionCursor;
  DivisionCycleStep m_currentStep;
  OMG::Troolean m_divisionCycleWithAns;
};

using ExpressionInputBar = Escher::TemplatedExpressionInputBar<LayoutField>;

}  // namespace Calculation
