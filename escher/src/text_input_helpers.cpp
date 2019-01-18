#include <escher/text_input_helpers.h>
#include <kandinsky/unicode/utf8_decoder.h>
#include <string.h>

namespace TextInputHelpers {

size_t CursorIndexInCommand(const char * text) {
  size_t index = 0;
  UTF8Decoder decoder(text);
  const char * currentPointer = text;
  CodePoint codePoint = decoder.nextCodePoint();
  const char * nextPointer = decoder.stringPosition();
  while (codePoint != KDCodePointNull) {
    if (codePoint == KDCodePointEmpty) {
      return index;
    }
    //TODO make sure changing empty / ' order was OK
    if (codePoint == '\'') {
      index+= nextPointer - currentPointer;
      currentPointer = nextPointer;
      codePoint = decoder.nextCodePoint();
      nextPointer = decoder.stringPosition();
      if (codePoint == '\'') {
        return index;
      }
      // Continue because we already incremented codePoint
      continue;
    }
    index+= nextPointer - currentPointer;
    currentPointer = nextPointer;
    codePoint = decoder.nextCodePoint();
    nextPointer = decoder.stringPosition();
  }
  return index;
}

}
