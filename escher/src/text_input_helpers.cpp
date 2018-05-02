#include <escher/text_input_helpers.h>
#include <ion/charset.h>
#include <string.h>

namespace TextInputHelpers {

int CursorIndexInCommand(const char * text) {
  int textLength = strlen(text);
  for (int i = 0; i < textLength - 1; i++) {
    if (text[i] == '\'' &&  text[i+1] == '\'') {
      return i + 1;
    } else if (text[i] == Ion::Charset::Empty) {
      return i;
    }
  }
  return textLength;
}

}
