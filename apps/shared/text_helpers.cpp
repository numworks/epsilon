#include "text_helpers.h"
#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <assert.h>

namespace Shared {
namespace TextHelpers {

void PadWithSpaces(char * buffer, int bufferSize, int * currentNumberOfChar, int maxGlyphLengthWithPadding) {
  assert(*currentNumberOfChar <= bufferSize);
  size_t currentGlyphLength = UTF8Helper::StringGlyphLength(buffer, *currentNumberOfChar);
  bool addedPadding = false;
  assert(UTF8Decoder::CharSizeOfCodePoint(' ') == 1);
  while ((int)currentGlyphLength < maxGlyphLengthWithPadding && *currentNumberOfChar < bufferSize) {
    *currentNumberOfChar = *currentNumberOfChar + UTF8Decoder::CodePointToChars(' ', buffer + *currentNumberOfChar, bufferSize - *currentNumberOfChar);
    addedPadding = true;
    currentGlyphLength++;
  }
  if (addedPadding) {
    buffer[*currentNumberOfChar-1] = 0;
  }
}

}
}
