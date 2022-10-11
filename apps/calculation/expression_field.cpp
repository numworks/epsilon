#include "expression_field.h"
#include <apps/i18n.h>
#include <poincare/symbol.h>
#include <poincare/horizontal_layout.h>
#include <poincare/code_point_layout.h>

using namespace Escher;
using namespace Poincare;

namespace Calculation {

bool ExpressionField::handleEvent(Ion::Events::Event event) {
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
    // The 'minus' symbol will be addded by ExpressionField::handleEvent
  }
  if (event == Ion::Events::Division
      && isEditing()) {
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

ExpressionField::DivisionCycleStep ExpressionField::currentStepOfDivisionCycling() {
  if (isEmpty()) {
    return DivisionCycleStep::Start;
  }
  bool cursorIsLeftOfLayout = false;
  char inputBuffer[k_divisionCycleCheckBufferSize];
  if (editionIsInTextField()) {
    if (strlen(m_textField.draftTextBuffer()) >= k_divisionCycleCheckBufferSize) {
      return DivisionCycleStep::NotCycling;
    }
    strlcpy(inputBuffer, m_textField.draftTextBuffer(), k_divisionCycleCheckBufferSize);
  } else {
    Layout layout = m_layoutField.layout();
    if (layout.numberOfChildren() > 1) {
      return DivisionCycleStep::NotCycling;
    }
    layout.serializeForParsing(inputBuffer, k_divisionCycleCheckBufferSize);
    cursorIsLeftOfLayout = m_layoutField.cursor()->isEquivalentTo(LayoutCursor(layout, Poincare::LayoutCursor::Position::Left));
  }
  if ((inputBuffer[0] == '/' && inputBuffer[1] == 0) || (!cursorIsLeftOfLayout && strcmp(k_serializedEmptyFraction, inputBuffer) == 0)) {
    return DivisionCycleStep::EmptyFraction;
  }
  if (strcmp(k_1DAnsFraction, inputBuffer) == 0 || strcmp(k_serializedAnsFraction, inputBuffer) == 0) {
    return DivisionCycleStep::AnsDivided;
  }
  if (strcmp(k_1DMixedFractionCommand, inputBuffer) == 0 || (cursorIsLeftOfLayout && strcmp(k_serializedEmptyFraction, inputBuffer) == 0)) {
    return DivisionCycleStep::MixedFraction;
  }
  return DivisionCycleStep::NotCycling;
}

bool ExpressionField::handleDivision() {
  /* The cycle is:
   * Ans fraction -> Empty fraction -> mixed fraction -> ans fraction -> etc.
   */
  Ion::Events::Event event = Ion::Events::Division;
  DivisionCycleStep currentDivisionCycleStep = currentStepOfDivisionCycling();
  if (currentDivisionCycleStep == DivisionCycleStep::MixedFraction || currentDivisionCycleStep == DivisionCycleStep::Start) {
    setText(Poincare::Symbol::k_ansAliases.mainAlias());
  } else if (currentDivisionCycleStep == DivisionCycleStep::AnsDivided) {
    setText("");
  } else if (currentDivisionCycleStep == DivisionCycleStep::EmptyFraction) {
    if (editionIsInTextField()) {
      setText(k_1DMixedFractionCommand);
      m_textField.setCursorLocation(m_textField.draftTextBuffer());
    } else {
      setText("");
      handleEventWithText(I18n::translate(I18n::Message::MixedFractionCommand));
    }
    return true;
  }
  return (::ExpressionField::handleEvent(event));
}

}