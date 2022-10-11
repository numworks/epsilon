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
    m_divisionCycleChoiceUpToDate = false;
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
    if (!m_divisionCycleChoiceUpToDate) {
      m_currentStep = DivisionCycleStep::Start;
      m_divisionCycleWithAns = isEmpty();
      m_divisionCycleChoiceUpToDate = true;
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

bool ExpressionField::createdEmptyFraction() {
  Layout layout = m_layoutField.cursor()->layout();
  assert(layout.isEmpty());

  Layout fraction;
  if (layout.parent().type() == LayoutNode::Type::FractionLayout) {
    fraction = layout.parent();
  }
  else {
    assert(layout.parent().type() == LayoutNode::Type::HorizontalLayout && layout.parent().numberOfChildren() == 1 && layout.parent().parent().type() == LayoutNode::Type::FractionLayout);
    fraction = layout.parent().parent();
  }
  assert(fraction.type() == LayoutNode::Type::FractionLayout);
  return fraction.childAtIndex(0).isEmpty() && fraction.childAtIndex(1).isEmpty();
}

bool ExpressionField::handleDivision() {
  /* The cycle is : 
   * 1) in a country with mixed fractions :
   *   1.1) when first operation : Start -> DenominatorOfAnsFraction -> NumeratorOfEmptyFraction -> MixedFraction -> DenominatorOfAnsFraction -> etc 
   *   1.2) otherwise :
   *      1.2.1) in 1D : Start -> DenominatorOfEmptyFraction -> NumeratorOfEmptyFraction -> MixedFraction -> DenominatorOfEmptyFraction -> etc 
   *      1.2.2) in 2D : Start -> NumeratorOfEmptyFraction -> DenominatorOfEmptyFraction -> MixedFraction -> NumeratorOfEmptyFraction -> etc 
   * 2) in a country without mixed fractions :
   *   2.1) when first operation : Start -> DenominatorOfAnsFraction -> NumeratorOfEmptyFraction -> DenominatorOfAnsFraction -> etc 
   *   2.2) otherwise : only default behavior which is 
   *      2.2.1) in 1D : DenominatorOfAnsFraction
   *      2.2.2) in 2D : NumeratorOfEmptyFraction */
  bool mixedFractionsEnabled = Poincare::Preferences::sharedPreferences()->mixedFractionsAreEnabled();
  Ion::Events::Event event = Ion::Events::Division;
  bool handled = true;

  if (m_divisionCycleWithAns) {
    // Cycles 1.1 and 1.2
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
        if (mixedFractionsEnabled) {
          m_currentStep = DivisionCycleStep::MixedFraction;
          if (editionIsInTextField()) {
            setText(" /");
            m_textField.setCursorLocation(m_textField.draftTextBuffer());
          } else {
            setText("");
            handleEventWithText(I18n::translate(I18n::Message::MixedFractionCommand));
          }
          return true;
        } else {
          m_currentStep = DivisionCycleStep::DenominatorOfAnsFraction;
          setText(Poincare::Symbol::k_ansAliases.mainAlias());
        }
        break;
      case DivisionCycleStep::MixedFraction :
        assert(mixedFractionsEnabled);
        m_currentStep = DivisionCycleStep::DenominatorOfAnsFraction;
        setText(Poincare::Symbol::k_ansAliases.mainAlias());
        break;
       case DivisionCycleStep::DenominatorOfEmptyFraction :
        assert(false);
        break;
    }
  } else if (mixedFractionsEnabled) {
    // Cycles 1.2.1 and 1.2.2
    switch (m_currentStep) {
      case DivisionCycleStep::Start :
        handled = (::ExpressionField::handleEvent(event));
        /* In 1D we always cycle
         * In 2D we cycle only if the default handleEvent created an 
         * empty fraction, to avoid the cases :
         *  - when we press Division after an expression, the default
              handleEvent create a fraction with the expression at the
              numerator and the cursor at the denominator
            - when we press Division before an expression, the default
              handleEvent create a fraction with the expresion at the 
              denominator and the cursor at the numerator
         * -> in both cases, we don't want to cycle */
        if (editionIsInTextField()) {
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
        } else if (createdEmptyFraction()) {
          m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        }
        if (!handled) {
          m_divisionCycleChoiceUpToDate = false;
        }
        return handled;
      case DivisionCycleStep::NumeratorOfEmptyFraction : 
        if (editionIsInTextField()) {
          m_currentStep = DivisionCycleStep::MixedFraction;
          handled = (::ExpressionField::handleEvent(Ion::Events::Space));
          assert(handled == true);
          event = Ion::Events::Left;
        } else {
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
          event = Ion::Events::Down;
        }
        break;
      case DivisionCycleStep::DenominatorOfEmptyFraction :
        if (editionIsInTextField()) {
          m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        } else {
          m_currentStep = DivisionCycleStep::MixedFraction;
        }
        event = Ion::Events::Left;
        break;
      case DivisionCycleStep::MixedFraction :
        assert(mixedFractionsEnabled);
        if (editionIsInTextField()) {
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
          handled = (::ExpressionField::handleEvent(Ion::Events::Right));
          assert(handled == true);
          handled = (::ExpressionField::handleEvent(Ion::Events::Backspace));
          assert(handled == true);
        } else {
          m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        }
        event = Ion::Events::Right;
        break;
      case DivisionCycleStep::DenominatorOfAnsFraction :
        assert(false);
        break;
    }
  }

  if (!handled) {
    m_divisionCycleChoiceUpToDate = false;
    return false;
  }
  return (::ExpressionField::handleEvent(event));
}

}