#include <escher/text_input_helpers.h>
#include <ion/unicode/utf8_decoder.h>
#include <string.h>

namespace TextInputHelpers {

const char * CursorPositionInCommand(const char * text, const char * stoppingPosition) {
  assert(stoppingPosition == nullptr || text <= stoppingPosition);
  UTF8Decoder decoder(text);
  const char * currentPointer = text;
  CodePoint codePoint = decoder.nextCodePoint();
  while ((stoppingPosition == nullptr || currentPointer < stoppingPosition) && codePoint != UCodePointNull) {
    if (codePoint == UCodePointEmpty) {
      return currentPointer;
    }
    //TODO make sure changing empty / ' order was OK
    if (codePoint == '\'') {
      currentPointer = decoder.stringPosition();
      codePoint = decoder.nextCodePoint();
      if (codePoint == '\'') {
        return currentPointer;
      }
      // Continue because we already incremented codePoint
      continue;
    }
    currentPointer = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  return currentPointer;
}

}
