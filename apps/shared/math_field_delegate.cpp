#include "math_field_delegate.h"

#include <apps/constant.h>
#include <apps/i18n.h>
#include <poincare/expression.h>

#include "continuous_function.h"
#include "poincare_helpers.h"

using namespace Poincare;
using namespace Escher;

namespace Shared {

bool AbstractMathFieldDelegate::fieldDidReceiveEvent(EditableField *field,
                                                     Responder *responder,
                                                     Ion::Events::Event event) {
  if (event == Ion::Events::XNT) {
    CodePoint defaultXNT = XNT();
    int XNTIndex = Ion::Events::repetitionFactor();
    if (XNTIndex > 0) {
      // Cycle through XNT CodePoints, starting from default code point position
      constexpr CodePoint XNTCodePoints[] = {
          ContinuousFunction::k_cartesianSymbol, 'n',
          ContinuousFunction::k_parametricSymbol,
          ContinuousFunction::k_polarSymbol};
      constexpr size_t k_numberOfCodePoints = std::size(XNTCodePoints);
      for (size_t i = 0; i < k_numberOfCodePoints; i++) {
        if (XNTCodePoints[i] == defaultXNT) {
          break;
        }
        XNTIndex++;
      }
      // Unknown default code point
      defaultXNT = XNTCodePoints[XNTIndex % k_numberOfCodePoints];
    }
    return field->addXNTCodePoint(defaultXNT);
  }
  return false;
}

CodePoint AbstractMathFieldDelegate::XNT() {
  return ContinuousFunction::k_cartesianSymbol;
}

bool AbstractMathFieldDelegate::isAcceptableExpression(const Expression exp) {
  return !exp.isUninitialized() && exp.type() != ExpressionNode::Type::Store &&
         ExpressionCanBeSerialized(exp, false, Poincare::Expression(),
                                   App::app()->localContext());
}

bool AbstractMathFieldDelegate::ExpressionCanBeSerialized(
    const Expression expression, bool replaceAns, Expression ansExpression,
    Context *context) {
  if (expression.isUninitialized()) {
    return false;
  }
  Expression exp = expression;
  if (replaceAns) {
    exp = expression.clone();
    Symbol ansSymbol = Symbol::Ans();
    exp = exp.replaceSymbolWithExpression(ansSymbol, ansExpression);
  }
  constexpr int maxSerializationSize = Constant::MaxSerializedExpressionSize;
  char buffer[maxSerializationSize];
  int length = PoincareHelpers::Serialize(exp, buffer, maxSerializationSize);
  /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
   * escaped before printing utterly the expression. */
  if (length >= maxSerializationSize - 1) {
    return false;
  }
  if (replaceAns) {
    exp = Expression::Parse(buffer, context);
    if (exp.isUninitialized()) {
      // The ans replacement made the expression unparsable
      return false;
    }
  }
  return true;
}

bool AbstractMathFieldDelegate::isAcceptableText(const char *text) {
  Expression exp = Expression::Parse(text, App::app()->localContext());
  return isAcceptableExpression(exp);
}

MathLayoutFieldDelegate *MathLayoutFieldDelegate::Default() {
  static MathLayoutFieldDelegate s_defaultMathLayoutFieldDelegate;
  return &s_defaultMathLayoutFieldDelegate;
}

bool MathLayoutFieldDelegate::layoutFieldShouldFinishEditing(
    LayoutField *layoutField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool MathLayoutFieldDelegate::layoutFieldDidReceiveEvent(
    LayoutField *layoutField, Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (layoutField->isEmpty()) {
      // Accept empty fields
      return false;
    }
    App *app = App::app();
    /* An acceptable layout has to be parsable and serialized in a fixed-size
     * buffer. We check all that here. */
    /* Step 1: Simple layout serialisation. Resulting texts can be parsed but
     * not displayed, like:
     * - 2a
     * - log_{2}(x) */
    constexpr int bufferSize = TextField::MaxBufferSize();
    char buffer[bufferSize];
    int length = layoutField->layout().serializeForParsing(buffer, bufferSize);
    if (length >= bufferSize - 1) {
      /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
       * escaped before printing utterly the expression. */
      app->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    /* Step 2: Parsing
     * Do not parse for assignment to detect if there is a syntax error, since
     * some errors could be missed.
     * Sometimes the field needs to be parsed for assignment but this is
     * done later, namely by ContinuousFunction::buildExpressionFromText.
     */
    Poincare::Expression e = Poincare::Expression::Parse(
        buffer, layoutField->context(), true, false);
    if (e.isUninitialized()) {
      // Unparsable expression
      app->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    /* Step 3: Expression serialization. Resulting texts are parseable and
     * displayable, like:
     * - 2*a
     * - log(x,2) */
    length =
        e.serialize(buffer, bufferSize,
                    Poincare::Preferences::sharedPreferences->displayMode());
    if (length >= bufferSize - 1) {
      // Same comment as before
      app->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    if (!isAcceptableExpression(e)) {
      app->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (fieldDidReceiveEvent(layoutField, layoutField, event)) {
    return true;
  }
  return false;
}

MathTextFieldDelegate *MathTextFieldDelegate::Default() {
  static MathTextFieldDelegate s_defaultMathTextFieldDelegate;
  return &s_defaultMathTextFieldDelegate;
}

bool MathTextFieldDelegate::textFieldShouldFinishEditing(
    AbstractTextField *textField, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool MathTextFieldDelegate::textFieldDidReceiveEvent(
    AbstractTextField *textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->shouldFinishEditing(event)) {
    if (textField->text()[0] == 0) {
      // Empty field, let the textfield handle the event
      return false;
    }
    if (!isAcceptableText(textField->text())) {
      App::app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (fieldDidReceiveEvent(textField, textField, event)) {
    return true;
  }
  return false;
}

template <typename T>
T MathTextFieldDelegate::ParseInputtedFloatValue(const char *text) {
  return PoincareHelpers::ParseAndSimplifyAndApproximateToScalar<T>(
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

template float MathTextFieldDelegate::ParseInputtedFloatValue<float>(
    const char *);
template double MathTextFieldDelegate::ParseInputtedFloatValue<double>(
    const char *);
template bool MathTextFieldDelegate::HasUndefinedValue(float, bool, bool);
template bool MathTextFieldDelegate::HasUndefinedValue(double, bool, bool);

}  // namespace Shared
