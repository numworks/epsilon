#include "expression_field_delegate_app.h"
#include <apps/i18n.h>
#include <escher/layout_field.h>
#include <poincare/expression.h>

using namespace Escher;
using namespace Poincare;

namespace Shared {

ExpressionFieldDelegateApp::ExpressionFieldDelegateApp(Snapshot * snapshot, ViewController * rootViewController) :
  TextFieldDelegateApp(snapshot, rootViewController),
  LayoutFieldDelegate()
{
}

bool ExpressionFieldDelegateApp::layoutFieldShouldFinishEditing(LayoutField * layoutField, Ion::Events::Event event) {
  return isFinishingEvent(event);
}

bool ExpressionFieldDelegateApp::layoutFieldDidReceiveEvent(LayoutField * layoutField, Ion::Events::Event event) {
  if (layoutField->isEditing() && layoutField->shouldFinishEditing(event)) {
    if (!layoutField->hasText()) {
      // Accept empty fields
      return false;
    }
    /* An acceptable layout has to be parsable and serialized in a fixed-size
     * buffer. We check all that here. */
    /* Step 1: Simple layout serialisation. Resulting texts can be parsed but
     * not displayed, like:
     * - 2a
     * - log_{2}(x) */
    constexpr int bufferSize = TextField::MaxBufferSize();
    char buffer[bufferSize];
    int length = layoutField->layout().serializeForParsing(buffer, bufferSize);
    if (length >= bufferSize-1) {
      /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
       * escaped before printing utterly the expression. */
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    /* Step 2: Parsing
     * Do not parse for assignment to detect if there is a syntax error, since
     * some errors could be missed.
     * Sometimes the field needs to be parsed for assignment but this is
     * done later, namely by ContinuousFunction::buildExpressionFromText.
     */
    Poincare::Expression e = Poincare::Expression::Parse(buffer, layoutField->context(), true, false);
    if (e.isUninitialized()) {
      // Unparsable expression
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    /* Step 3: Expression serialization. Resulting texts are parseable and
     * displayable, like:
     * - 2*a
     * - log(x,2) */
    length = e.serialize(buffer, bufferSize, Poincare::Preferences::sharedPreferences()->displayMode());
    if (length >= bufferSize-1) {
      // Same comment as before
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    if (!isAcceptableExpression(e)) {
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (fieldDidReceiveEvent(layoutField, layoutField, event)) {
    return true;
  }
  return false;
}

bool ExpressionFieldDelegateApp::isAcceptableExpression(const Expression exp) {
  /* Override TextFieldDelegateApp because most ExpressionFieldDelegateApp
   * accept comparison operatoras. They should also be serializeable. */
  return !exp.isUninitialized() && exp.type() != ExpressionNode::Type::Store && TextFieldDelegateApp::ExpressionCanBeSerialized(exp, false, Poincare::Expression(), localContext());
}

bool ExpressionFieldDelegateApp::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Sto || event == Ion::Events::Var) {
    storeValue();
    return true;
  }
  return TextFieldDelegateApp::handleEvent(event);
}

void ExpressionFieldDelegateApp::storeValue(const char * text) {
  if (m_modalViewController.isDisplayingModal()) {
    return;
  }
  m_storeMenuController.setText(text);
  m_storeMenuController.open();
}

bool ExpressionFieldDelegateApp::isStoreMenuOpen() const {
  return m_modalViewController.currentModalViewController() == &m_storeMenuController;
}

}
