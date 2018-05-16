#include <escher/text_input_helpers.h>
#include <ion/charset.h>
#include <string.h>

namespace TextInputHelpers {

size_t CursorIndexInCommand(const char * text) {
  size_t textLength = strlen(text);
  for (size_t i = 0; i < textLength - 1; i++) {
    if (text[i] == '\'' &&  text[i+1] == '\'') {
      return i + 1;
    } else if (text[i] == Ion::Charset::Empty) {
      return i;
    }
  }
  return textLength;
}

}
