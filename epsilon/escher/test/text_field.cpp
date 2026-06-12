#include <escher/text_field.h>
#include <ion/events.h>
#include <quiz.h>

using namespace Escher;

QUIZ_CASE(escher_text_field_insert_text_strips_newline) {
  // Regression: inserting text with '\n' before a multibyte UTF-8 char must
  // not place the cursor inside that char (leading to a crash on Left).
  constexpr size_t bufferSize = TextField::MaxBufferSize();
  char buffer[bufferSize];
  TextField field(nullptr, buffer, bufferSize);

  field.setText("π");
  field.setEditing(true);
  field.handleEvent(Ion::Events::Left);

  // '\n' is stripped from "0\n" before insertion; cursor offset must account
  // for that so it doesn't land in the middle of π (0xCF 0x80).
  field.handleEventWithText("0\n");

  // Without the fix this crashes with assert(leadingOnes == 1) in
  // UTF8Decoder::previousCodePoint.
  field.handleEvent(Ion::Events::Left);
}
