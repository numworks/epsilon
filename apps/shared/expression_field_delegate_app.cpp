#include "expression_field_delegate_app.h"
#include <escher.h>
#include <apps/i18n.h>
#include <poincare/expression.h>

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
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    /* An acceptable layout has to be parsable and serialized in a fixed-size
     * buffer. We check all that here. */
    /* Step 1: Simple layout serialisation. Resulting texts can be parsed but
     * not displayed, like:
     * - 2a
     * - log_{2}(x) */
    constexpr int bufferSize = TextField::maxBufferSize();
    char buffer[bufferSize];
    int length = layoutField->layout().serializeForParsing(buffer, bufferSize);
    if (length >= bufferSize-1) {
      /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
       * escaped before printing utterly the expression. */
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    // Step 2: Parsing
    Poincare::Expression e = Poincare::Expression::Parse(buffer, layoutField->context());
    if (e.isUninitialized()) {
      // Unparsable expression
      displayWarning(I18n::Message::SyntaxError);
      return true;
    }
    /* Step 3: Expression serialization. Tesulting texts are parseable and
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

}
