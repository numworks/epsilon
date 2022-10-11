#include "expression_field.h"
#include <apps/i18n.h>
#include <poincare/symbol.h>
#include <poincare/horizontal_layout.h>
#include <poincare/code_point_layout.h>

using namespace Escher;
using namespace Poincare;

namespace Calculation {

bool ExpressionField::handleEvent(Ion::Events::Event event) {
  if (event != Ion::Events::Division) {
    m_divisionStepUpToDate = false;
  }
  if (event == Ion::Events::Back) {
    return false;
  }
  if (event == Ion::Events::Ans) {
    handleEventWithText(Poincare::Symbol::k_ansAliases.mainAlias());
    return true;
  }
  if (isEditing() && isEmpty() &&
      (event == Ion::Events::Multiplication ||
       event == Ion::Events::Plus ||
       event == Ion::Events::Power ||
       event == Ion::Events::Square ||
       event == Ion::Events::Sto)) {
    handleEventWithText(Poincare::Symbol::k_ansAliases.mainAlias());
  }
  if (event == Ion::Events::Minus
      && isEditing()
      && fieldContainsSingleMinusSymbol()) {
    setText(Poincare::Symbol::k_ansAliases.mainAlias());
    // The Minus symbol will be addded by (::ExpressionField::handleEvent)
  }
  if (event == Ion::Events::Division
      && isEditing()) {
    if (!m_divisionStepUpToDate) {
      m_currentStep = isEmpty() ? DivisionCycleStep::Start : DivisionCycleStep::NotCycling;
      m_divisionStepUpToDate = true;
    }
    return handleDivision();
  }
  return (::ExpressionField::handleEvent(event));
}

bool ExpressionField::fieldContainsSingleMinusSymbol() const {
  if (editionIsInTextField()) {
    const char * inputBuffer = m_textField.draftTextBuffer();
    return (inputBuffer[0] == '-' && inputBuffer[1] == 0);
  } else {
    Layout layout = m_layoutField.layout();
    if (layout.type() == LayoutNode::Type::HorizontalLayout
        && layout.numberOfChildren() == 1) {
      Layout child = layout.childAtIndex(0);
      if (child.type() == LayoutNode::Type::CodePointLayout) {
        return static_cast<CodePointLayout &>(child).codePoint() == '-';
      }
    }
    return false;
  }
}

bool ExpressionField::handleDivision() {
  /* The cycle is:
   * Ans fraction -> Empty fraction -> mixed fraction -> ans fraction -> etc.
   */
  Ion::Events::Event event = Ion::Events::Division;
  switch (m_currentStep) {
    case DivisionCycleStep::Start:
      assert(isEmpty());
      m_currentStep = DivisionCycleStep::DenominatorOfAnsFraction;
      setText(Poincare::Symbol::k_ansAliases.mainAlias());
      break;
    case DivisionCycleStep::DenominatorOfAnsFraction : 
      m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
      setText("");
      break;
    case DivisionCycleStep::NumeratorOfEmptyFraction :
      m_currentStep = DivisionCycleStep::MixedFraction;
      if (editionIsInTextField()) {
        setText(" /");
        m_textField.setCursorLocation(m_textField.draftTextBuffer());
      } else {
        setText("");
        handleEventWithText(I18n::translate(I18n::Message::MixedFractionCommand));
      }
      return true;
    case DivisionCycleStep::MixedFraction :
      m_currentStep = DivisionCycleStep::DenominatorOfAnsFraction;
      setText(Poincare::Symbol::k_ansAliases.mainAlias());
      break;
  }
  return (::ExpressionField::handleEvent(event));
}

}