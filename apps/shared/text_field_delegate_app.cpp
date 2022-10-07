#include "text_field_delegate_app.h"
#include "continuous_function.h"
#include <apps/apps_container_helper.h>
#include <apps/constant.h>
#include <apps/shared/poincare_helpers.h>
#include <cmath>
#include <string.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

Context * TextFieldDelegateApp::localContext() {
  return AppsContainerHelper::sharedAppsContainerGlobalContext();
}

CodePoint TextFieldDelegateApp::XNT() {
  return ContinuousFunction::k_cartesianSymbol;
}

bool TextFieldDelegateApp::textFieldShouldFinishEditing(AbstractTextField * textField, Ion::Events::Event event) {
  return isFinishingEvent(event);
}

bool TextFieldDelegateApp::textFieldDidReceiveEvent(AbstractTextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->shouldFinishEditing(event)) {
    if (textField->text()[0] == 0) {
      // Empty field, let the textfield handle the event
      return false;
    }
    if (!isAcceptableText(textField->text())) {
      return true;
    }
  }
  if (fieldDidReceiveEvent(textField, textField, event)) {
    return true;
  }
  return false;
}


bool TextFieldDelegateApp::isAcceptableText(const char * text) {
  Expression exp = Expression::Parse(text, localContext());
  bool isAcceptable = isAcceptableExpression(exp);
  if (!isAcceptable) {
    displayWarning(I18n::Message::SyntaxError);
  }
  return isAcceptable;
}

template<typename T>
bool TextFieldDelegateApp::hasUndefinedValue(const char * text, T * value, bool enablePlusInfinity, bool enableMinusInfinity) {
  *value = PoincareHelpers::ParseAndSimplifyAndApproximateToScalar<T>(text, localContext());
  bool isUndefined = std::isnan(*value)
    || (!enablePlusInfinity && *value > 0 && std::isinf(*value))
    || (!enableMinusInfinity && *value < 0 && std::isinf(*value));
  if (isUndefined) {
    displayWarning(I18n::Message::UndefinedValue);
  }
  return isUndefined;
}

/* Protected */

TextFieldDelegateApp::TextFieldDelegateApp(Snapshot * snapshot, ViewController * rootViewController) :
  InputEventHandlerDelegateApp(snapshot, rootViewController),
  TextFieldDelegate()
{
}

bool TextFieldDelegateApp::fieldDidReceiveEvent(EditableField * field, Responder * responder, Ion::Events::Event event) {
  if (event == Ion::Events::XNT) {
    CodePoint defaultXNT = XNT();
    int XNTIndex = Ion::Events::repetitionFactor();
    if (XNTIndex > 0) {
      // Cycle through XNT CodePoints, starting from default code point position
      constexpr CodePoint XNTCodePoints[] = {ContinuousFunction::k_cartesianSymbol, 'n', ContinuousFunction::k_parametricSymbol, ContinuousFunction::k_polarSymbol};
      constexpr size_t k_numberOfCodePoints = sizeof(XNTCodePoints)/sizeof(CodePoint);
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

bool TextFieldDelegateApp::isFinishingEvent(Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool TextFieldDelegateApp::isAcceptableExpression(const Expression exp) {
  // Most TextFieldDelegateApps shouldn't accept Store.
  return !(exp.isUninitialized() || exp.type() == ExpressionNode::Type::Store);
}

bool TextFieldDelegateApp::ExpressionCanBeSerialized(const Expression expression, bool replaceAns, Expression ansExpression, Context * context) {
  if (expression.isUninitialized()) {
    return false;
  }
  Expression exp = expression;
  if (replaceAns){
    exp = expression.clone();
    Symbol ansSymbol = Symbol::Ans();
    exp = exp.replaceSymbolWithExpression(ansSymbol, ansExpression);
  }
  constexpr int maxSerializationSize = Constant::MaxSerializedExpressionSize;
  char buffer[maxSerializationSize];
  int length = PoincareHelpers::Serialize(exp, buffer, maxSerializationSize);
  /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
   * escaped before printing utterly the expression. */
  if (length >= maxSerializationSize-1) {
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

template bool TextFieldDelegateApp::hasUndefinedValue(const char *, float *, bool, bool);
template bool TextFieldDelegateApp::hasUndefinedValue(const char *, double *, bool, bool);

}
