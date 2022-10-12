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

void ExpressionField::createAnsFraction() {
  assert(m_currentStep == DivisionCycleStep::Start || m_currentStep == DivisionCycleStep::MixedFraction);
  m_currentStep = DivisionCycleStep::DenominatorOfAnsFraction;
  setText(Poincare::Symbol::k_ansAliases.mainAlias());
}

bool ExpressionField::createdEmptyFraction() {
  Layout pointedLayout = m_layoutField.cursor()->layout();
  assert(pointedLayout.isEmpty());

  Layout fraction;
  if (pointedLayout.parent().type() == LayoutNode::Type::FractionLayout) {
    fraction = pointedLayout.parent();
  } else {
    assert(pointedLayout.parent().type() == LayoutNode::Type::HorizontalLayout && pointedLayout.parent().numberOfChildren() == 1 && pointedLayout.parent().parent().type() == LayoutNode::Type::FractionLayout);
    fraction = pointedLayout.parent().parent();
  }
  assert(fraction.type() == LayoutNode::Type::FractionLayout);
  return fraction.childAtIndex(0).isEmpty() && fraction.childAtIndex(1).isEmpty();
}

bool ExpressionField::handleDivision() {
  /* The cycle is: 
   * 1) in a country with mixed fractions:
   *   1.1) when first operation : Start -> DenominatorOfAnsFraction -> NumeratorOfEmptyFraction -> MixedFraction -> DenominatorOfAnsFraction -> etc 
   *   1.2) otherwise:
   *      1.2.1) in 1D: Start -> DenominatorOfEmptyFraction -> NumeratorOfEmptyFraction -> MixedFraction -> DenominatorOfEmptyFraction -> etc 
   *      1.2.2) in 2D: Start -> NumeratorOfEmptyFraction -> DenominatorOfEmptyFraction -> MixedFraction -> NumeratorOfEmptyFraction -> etc 
   * 2) in a country without mixed fractions :
   *   2.1) when first operation: Start -> DenominatorOfAnsFraction -> NumeratorOfEmptyFraction -> DenominatorOfAnsFraction -> etc 
   *   2.2) otherwise: only default behavior which is 
   *      2.2.1) in 1D: DenominatorOfAnsFraction
   *      2.2.2) in 2D: NumeratorOfEmptyFraction */
  bool mixedFractionsEnabled = Poincare::Preferences::sharedPreferences()->mixedFractionsAreEnabled();
  bool editionIn1D = editionIsInTextField();
  Ion::Events::Event event = Ion::Events::Division;
  bool handled = true;

  if (m_divisionCycleWithAns) {
    // Cycles 1.1 and 1.2
    switch (m_currentStep) {
      case DivisionCycleStep::Start:
        // Start -> DenominatorOfAnsFraction
        assert(isEmpty());
        createAnsFraction();
        break;
      case DivisionCycleStep::DenominatorOfAnsFraction : 
        // DenominatorOfAnsFraction -> NumeratorOfEmptyFraction
        m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        setText("");
        break;
      case DivisionCycleStep::NumeratorOfEmptyFraction :
        // NumeratorOfEmptyFraction -> MixedFraction
        m_currentStep = DivisionCycleStep::MixedFraction;
        if (mixedFractionsEnabled) {
          if (editionIn1D) {
            setText(" /");
            m_textField.setCursorLocation(m_textField.draftTextBuffer());
          } else {
            setText("");
            handleEventWithText(I18n::translate(I18n::Message::MixedFractionCommand));
          }
          return true;
        }
        /* If mixed fractions are not enabled, fall under next case
         * in order to skip the MixedFraction step */
      case DivisionCycleStep::MixedFraction :
        // MixedFraction -> DenominatorOfAnsFraction
        createAnsFraction();
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
         * empty fraction, to avoid the cases:
         *  - when we press Division after an expression, the default
              handleEvent creates a fraction with the expression at the
              numerator and the cursor at the denominator
            - when we press Division before an expression, the default
              handleEvent creates a fraction with the expresion at the 
              denominator and the cursor at the numerator
         * -> in both cases, we don't want to cycle */
        if (editionIn1D) {
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
        } else if (createdEmptyFraction()) {
          m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        }
        if (!handled) {
          m_divisionCycleChoiceUpToDate = false;
        }
        return handled;
      case DivisionCycleStep::NumeratorOfEmptyFraction : 
        if (editionIn1D) {
          m_currentStep = DivisionCycleStep::MixedFraction;
          handled = (::ExpressionField::handleEvent(Ion::Events::Space)); // TODO : OR handleEventWithText(" ");
          assert(handled);
          event = Ion::Events::Left;
        } else {
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
          event = Ion::Events::Down;
        }
        break;
      case DivisionCycleStep::DenominatorOfEmptyFraction :
        if (editionIn1D) {
          m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        } else {
          m_currentStep = DivisionCycleStep::MixedFraction;
        }
        event = Ion::Events::Left;
        break;
      case DivisionCycleStep::MixedFraction :
        assert(mixedFractionsEnabled);
        if (editionIn1D) {
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
          handled = (::ExpressionField::handleEvent(Ion::Events::Right)); // TODO : OR m_textField.moveCursorRight(); but protected in TextInput
          assert(handled);
          handled = (::ExpressionField::handleEvent(Ion::Events::Backspace)); // TODO : OR m_textField.removePreviousGlyph();
          assert(handled);
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
  return (::ExpressionField::handleEvent(event));
}

}