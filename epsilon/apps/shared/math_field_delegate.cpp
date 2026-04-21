#include "math_field_delegate.h"

#include <apps/constant.h>
#include <apps/global_preferences.h>
#include <apps/i18n.h>
#include <omg/unreachable.h>
#include <poincare/code_points.h>
#include <poincare/user_expression.h>

#include "syntax_error_message.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

bool AbstractMathFieldDelegate::handleEventForField(EditableField* field,
                                                    Ion::Events::Event event) {
  if (event == Ion::Events::XNT) {
    bool inserted = field->handleXNT(m_currentXNTIndex, defaultXNT());
    if (!inserted) {
      m_currentXNTIndex = -1;
    }
    return true;
  }
  return false;
}

void AbstractMathFieldDelegate::updateXNTIndex(Ion::Events::Event event) {
  if (event.isKeyPress()) {
    m_currentXNTIndex = event == Ion::Events::XNT ? m_currentXNTIndex + 1 : -1;
  }
}

CodePoint AbstractMathFieldDelegate::defaultXNT() {
  return CodePoints::k_cartesianSymbol;
}

bool AbstractMathFieldDelegate::isAcceptableExpression(
    const UserExpression& exp, const SymbolContext& symbolContext) {
  assert(!exp.isUninitialized());
  return !exp.isStore();
}

void AbstractMathFieldDelegate::displayWarningForSyntaxError(
    SyntaxError error) const {
  App::app()->displayWarning(MessageForSyntaxError(error));
}

MathLayoutFieldDelegate* MathLayoutFieldDelegate::Default() {
  static MathLayoutFieldDelegate s_defaultMathLayoutFieldDelegate;
  return &s_defaultMathLayoutFieldDelegate;
}

bool MathLayoutFieldDelegate::layoutFieldDidReceiveEvent(
    LayoutField* layoutField, Ion::Events::Event event) {
  return LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField, event)
             ? true
             : handleEventForField(layoutField, event);
}

bool MathLayoutFieldDelegate::isAcceptableLayoutOrWarning(
    const Poincare::Layout& layout,
    const Poincare::SymbolContext& symbolContext) {
  if (layout.isEmpty()) {
    // Accept empty layouts
    return true;
  }
  UserExpression::ParseOutput result =
      UserExpression::TryParse(layout, symbolContext);
  UserExpression exp = result.expression;
  if (exp.isUninitialized()) {
    displayWarningForSyntaxError(result.syntaxError);
    return false;
  }
  if (!isAcceptableExpression(exp, symbolContext)) {
    displayWarningForSyntaxError(
        Poincare::SyntaxError::NonAcceptableExpression);
    return false;
  }
  return true;
}

bool MathLayoutFieldDelegate::layoutFieldDidFinishEditing(
    LayoutField* layoutField, Ion::Events::Event event) {
  assert(!layoutField->isEditing());
  return isAcceptableLayoutOrWarning(layoutField->layout(), *context());
}

MathTextFieldDelegate* MathTextFieldDelegate::Default() {
  static MathTextFieldDelegate s_defaultMathTextFieldDelegate;
  return &s_defaultMathTextFieldDelegate;
}

bool MathTextFieldDelegate::textFieldDidReceiveEvent(
    AbstractTextField* textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event)
             ? true
             : handleEventForField(textField, event);
}

bool MathTextFieldDelegate::textFieldDidFinishEditing(
    AbstractTextField* textField, Ion::Events::Event event) {
  /* textFieldDidFinishEditing implementation has been removed because it is
   * unused. It can be added back if needed. */
  OMG_UNREACHABLE;
  return false;
}

template <typename T>
T MathTextFieldDelegate::ParseInputFloatValue(const char* text) {
  return UserExpression::ParseAndSimplifyAndApproximateToRealScalar<T>(
      text, SimplificationParameters{
                .user = {
                    .complexFormat = SharedGlobalPrefs().complexFormat(),
                    .angleUnit = SharedGlobalPrefs().angleUnit(),
                    .context = App::app()->localContext(),
                }});
}

template <typename T>
bool MathTextFieldDelegate::HasUndefinedValue(T value, bool enablePlusInfinity,
                                              bool enableMinusInfinity) {
  bool isUndefined = std::isnan(value) ||
                     (!enablePlusInfinity && value > 0 && std::isinf(value)) ||
                     (!enableMinusInfinity && value < 0 && std::isinf(value));
  if (isUndefined) {
    App::app()->displayWarning(I18n::Message::UndefinedValue);
  }
  return isUndefined;
}

template float MathTextFieldDelegate::ParseInputFloatValue<float>(const char*);
template double MathTextFieldDelegate::ParseInputFloatValue<double>(
    const char*);
template bool MathTextFieldDelegate::HasUndefinedValue(float, bool, bool);
template bool MathTextFieldDelegate::HasUndefinedValue(double, bool, bool);

}  // namespace Shared
