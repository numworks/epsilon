#include <escher/text_input_helpers.h>
#include <ion/unicode/utf8_decoder.h>
#include <string.h>

namespace Escher {
namespace TextInputHelpers {

size_t CursorIndexInCommand(const char * text, const char * stoppingPosition, bool ignoreCodePointSytem) {
  assert(stoppingPosition == nullptr || text <= stoppingPosition);
  UTF8Decoder decoder(text);
  const char * currentPointer = text;
  int numberOfIgnoredChar = 0;
  CodePoint codePoint = decoder.nextCodePoint();
  while ((stoppingPosition == nullptr || currentPointer < stoppingPosition) && codePoint != UCodePointNull) {
    if (ignoreCodePointSytem && codePoint == UCodePointSystem) {
      assert(UTF8Decoder::CharSizeOfCodePoint(UCodePointSystem) == 1);
      numberOfIgnoredChar++;
    } else if (codePoint == UCodePointEmpty) {
      return currentPointer - text - numberOfIgnoredChar;
    } else if (codePoint == '\'') {
      //TODO make sure changing empty / ' order was OK
      currentPointer = decoder.stringPosition();
      codePoint = decoder.nextCodePoint();
      if (codePoint == '\'') {
        return currentPointer - text - numberOfIgnoredChar;
      }
      // Continue because we already incremented codePoint
      continue;
    }
    currentPointer = decoder.stringPosition();
    codePoint = decoder.nextCodePoint();
  }
  return currentPointer - text - numberOfIgnoredChar;
}

}
}
