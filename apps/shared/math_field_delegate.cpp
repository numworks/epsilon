#include "math_field_delegate.h"

#include <apps/constant.h>
#include <apps/i18n.h>
#include <poincare/expression.h>

#include "continuous_function.h"
#include "poincare_helpers.h"
#include "sequence.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

bool AbstractMathFieldDelegate::handleEventForField(EditableField *field,
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
  return ContinuousFunction::k_cartesianSymbol;
}

bool AbstractMathFieldDelegate::isAcceptableExpression(const UserExpression exp,
                                                       Context *context) {
  return !exp.isUninitialized() && exp.type() != ExpressionNode::Type::Store;
}

bool AbstractMathFieldDelegate::isAcceptableText(const char *text,
                                                 Context *context) {
  /* Parsing
   * Do not parse for assignment to detect if there is a syntax error, since
   * some errors could be missed.
   * Sometimes the field needs to be parsed for assignment but this is
   * done later, namely by ContinuousFunction::buildExpressionFromText. */
  UserExpression exp = UserExpression::Parse(text, context);
  if (exp.isUninitialized()) {
    // Unparsable expression
    return false;
  }
  /* Expression serialization.
   * Resulting texts are parseable and displayable, like:
   * - 2*a
   * - log(x,2) */
  constexpr int bufferSize = TextField::MaxBufferSize();
  char buffer[bufferSize];
  int length = exp.serialize(buffer, bufferSize,
                             Preferences::SharedPreferences()->displayMode());
  if (length >= bufferSize - 1) {
    /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
     * escaped before printing utterly the expression. */
    return false;
  }
  return isAcceptableExpression(exp, context);
}

MathLayoutFieldDelegate *MathLayoutFieldDelegate::Default() {
  static MathLayoutFieldDelegate s_defaultMathLayoutFieldDelegate;
  return &s_defaultMathLayoutFieldDelegate;
}

bool MathLayoutFieldDelegate::layoutFieldDidReceiveEvent(
    LayoutField *layoutField, Ion::Events::Event event) {
  return LayoutFieldDelegate::layoutFieldDidReceiveEvent(layoutField, event)
             ? true
             : handleEventForField(layoutField, event);
}

bool MathLayoutFieldDelegate::isAcceptableLayout(Layout layout,
                                                 Poincare::Context *context) {
  if (layout.isEmpty()) {
    // Accept empty layouts
    return true;
  }
  UserExpression exp = UserExpression::Parse(layout, context);
  return isAcceptableExpression(exp, context);
}

bool MathLayoutFieldDelegate::layoutFieldDidFinishEditing(
    LayoutField *layoutField, Ion::Events::Event event) {
  assert(!layoutField->isEditing());
  if (!isAcceptableLayout(layoutField->layout(), context())) {
    App::app()->displayWarning(I18n::Message::SyntaxError);
    return false;
  }
  return true;
}

MathTextFieldDelegate *MathTextFieldDelegate::Default() {
  static MathTextFieldDelegate s_defaultMathTextFieldDelegate;
  return &s_defaultMathTextFieldDelegate;
}

bool MathTextFieldDelegate::textFieldDidReceiveEvent(
    AbstractTextField *textField, Ion::Events::Event event) {
  return TextFieldDelegate::textFieldDidReceiveEvent(textField, event)
             ? true
             : handleEventForField(textField, event);
}

bool MathTextFieldDelegate::textFieldDidFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  assert(!textField->isEditing());
  if (textField->text()[0] != 0 &&
      !isAcceptableText(textField->text(), App::app()->localContext())) {
    App::app()->displayWarning(I18n::Message::SyntaxError);
    return false;
  }
  return true;
}

template <typename T>
T MathTextFieldDelegate::ParseInputFloatValue(const char *text) {
  return Expression::ParseAndSimplifyAndApproximateToScalar<T>(
      text, App::app()->localContext());
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

template float MathTextFieldDelegate::ParseInputFloatValue<float>(const char *);
template double MathTextFieldDelegate::ParseInputFloatValue<double>(
    const char *);
template bool MathTextFieldDelegate::HasUndefinedValue(float, bool, bool);
template bool MathTextFieldDelegate::HasUndefinedValue(double, bool, bool);

}  // namespace Shared
