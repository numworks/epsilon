#include "layout_field.h"

#include <apps/i18n.h>
#include <poincare/code_point_layout.h>
#include <poincare/horizontal_layout.h>
#include <poincare/symbol.h>

using namespace Poincare;

namespace Calculation {

void LayoutField::updateCursorBeforeInsertion() {
  if (m_insertionCursor.isUninitialized()) {
    return;
  }
  KDSize previousSize = minimalSizeForOptimalDisplay();
  *cursor() = m_insertionCursor;
  reload(previousSize);
  resetInsertionCursor();
}

bool LayoutField::handleEvent(Ion::Events::Event event) {
  /* If the user tries to go back in history to insert a layout in the field,
   * we want to remember where the up sequence started so that the insertion
   * is done here. */
  if (event == Ion::Events::Up) {
    if (m_insertionCursor.isUninitialized()) {
      m_insertionCursor = *cursor();
      /* Ensure insertion cursor will stay valid even when the current layout is
       * exited */
      // m_insertionCursor.prepareForExitingPosition();
    }
  } else if (event.isKeyPress()) {
    resetInsertionCursor();
  }
  if (event != Ion::Events::Division && event.isKeyPress()) {
    m_divisionCycleWithAns = TrinaryBoolean::Unknown;
  }
  if (event == Ion::Events::Back) {
    return false;
  }
  if (isEditing() && isEmpty() &&
      (event == Ion::Events::Multiplication || event == Ion::Events::Plus ||
       event == Ion::Events::Power || event == Ion::Events::Square ||
       event == Ion::Events::Sto)) {
    insertText(Symbol::k_ansAliases.mainAlias());
  }
  if (event == Ion::Events::Minus && isEditing() &&
      fieldContainsSingleMinusSymbol()) {
    setText(Symbol::k_ansAliases.mainAlias());
    // The Minus symbol will be addded by Escher::LayoutField::handleEvent
  }
  if (event == Ion::Events::Division && isEditing()) {
    if (m_divisionCycleWithAns == TrinaryBoolean::Unknown) {
      m_currentStep = DivisionCycleStep::Start;
      m_divisionCycleWithAns =
          isEmpty() ? TrinaryBoolean::True : TrinaryBoolean::False;
    }
    return handleDivision();
  }
  return Escher::LayoutField::handleEvent(event);
}

bool LayoutField::fieldContainsSingleMinusSymbol() const {
  using PoincareJ::operator""_l;
  return layout().tree()->treeIsIdenticalTo("-"_l);
}

bool LayoutField::handleDivision() {
  assert(m_divisionCycleWithAns != TrinaryBoolean::Unknown);
  bool mixedFractionsEnabled =
      Preferences::SharedPreferences()->mixedFractionsAreEnabled();
  Ion::Events::Event event = Ion::Events::Division;

  if (m_divisionCycleWithAns == TrinaryBoolean::True) {
    /* When we are in the "Ans" case, the cycle is the following :
     * Start -> DenominatorOfAnsFraction -> NumeratorOfEmptyFraction (->
     * MixedFraction) -> DenominatorOfAnsFraction -> etc with the mixed fraction
     * step only kept when the country enables it */
    clearLayout();
    switch (m_currentStep) {
      case DivisionCycleStep::DenominatorOfAnsFraction:
        // DenominatorOfAnsFraction -> NumeratorOfEmptyFraction
        m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        break;
      case DivisionCycleStep::NumeratorOfEmptyFraction:
        // NumeratorOfEmptyFraction -> MixedFraction
        m_currentStep = DivisionCycleStep::MixedFraction;
        if (mixedFractionsEnabled) {
          return handleEventWithText(
              I18n::translate(I18n::Message::MixedFractionCommand));
        }
        /* If mixed fractions are not enabled, fall under next case
         * in order to skip the MixedFraction step */
      default:
        // Start / MixedFraction -> DenominatorOfAnsFraction
        assert(m_currentStep == DivisionCycleStep::Start ||
               m_currentStep == DivisionCycleStep::MixedFraction);
        m_currentStep = DivisionCycleStep::DenominatorOfAnsFraction;
        insertText(Symbol::k_ansAliases.mainAlias());
    }
  } else if (mixedFractionsEnabled) {
    assert(m_divisionCycleWithAns == TrinaryBoolean::False);
    bool editionIn1D = linearMode();
    bool handled = true;
    /* When we are in NOT the "Ans" case, the cycle is the following :
     *   - in 1D: Start -> DenominatorOfEmptyFraction ->
     * NumeratorOfEmptyFraction   -> MixedFraction -> DenominatorOfEmptyFraction
     * -> etc
     *   - in 2D: Start -> NumeratorOfEmptyFraction   ->
     * DenominatorOfEmptyFraction -> MixedFraction -> NumeratorOfEmptyFraction
     * -> etc and we do NOT cycle when the country doesn't enable mixed
     * fractions (because without the mixed fraction step, the cycle would only
     * switch between the numerator and the denominator of an empty fraction,
     * which is not the wanted behavior when pressing the Division key) */
    switch (m_currentStep) {
      case DivisionCycleStep::Start:
        handled = Escher::LayoutField::handleEvent(event);
        /* In 1D we always cycle
         * In 2D we cycle only if the default handleEvent created an empty
         * fraction */
        if (editionIn1D) {
          // 1D: Start -> DenominatorOfEmptyFraction
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
        } else if (cursor()->isAtNumeratorOfEmptyFraction()) {
          // 2D: Start -> NumeratorOfEmptyFraction
          m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        }
        if (!handled) {
          m_divisionCycleWithAns = TrinaryBoolean::Unknown;
        }
        return handled;
      case DivisionCycleStep::NumeratorOfEmptyFraction:
        if (editionIn1D) {
          // 1D: NumeratorOfEmptyFraction -> MixedFraction
          handled = Escher::LayoutField::handleEvent(Ion::Events::Space);
          if (!handled) {
            // Not enough space
            return false;
          }
          m_currentStep = DivisionCycleStep::MixedFraction;
          event = Ion::Events::Left;
        } else {
          // 2D: NumeratorOfEmptyFraction -> DenominatorOfEmptyFraction
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
          event = Ion::Events::Down;
        }
        break;
      case DivisionCycleStep::DenominatorOfEmptyFraction:
        if (editionIn1D) {
          // 1D: DenominatorOfEmptyFraction -> NumeratorOfEmptyFraction
          m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        } else {
          // 2D: DenominatorOfEmptyFraction -> MixedFraction
          m_currentStep = DivisionCycleStep::MixedFraction;
        }
        event = Ion::Events::Left;
        break;
      default:
        assert(m_currentStep == DivisionCycleStep::MixedFraction &&
               mixedFractionsEnabled);
        if (editionIn1D) {
          // 1D: MixedFraction -> DenominatorOfEmptyFraction
          m_currentStep = DivisionCycleStep::DenominatorOfEmptyFraction;
          handled = Escher::LayoutField::handleEvent(Ion::Events::Right);
          assert(handled);
          handled = Escher::LayoutField::handleEvent(Ion::Events::Backspace);
          assert(handled);
        } else {
          // 2D: MixedFraction -> NumeratorOfEmptyFraction
          m_currentStep = DivisionCycleStep::NumeratorOfEmptyFraction;
        }
        event = Ion::Events::Right;
        break;
    }
  }
  return Escher::LayoutField::handleEvent(event);
}

}  // namespace Calculation
