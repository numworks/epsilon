#include <escher/text_input_helpers.h>
#include <string.h>

namespace TextInputHelpers {

int CursorIndexInCommand(const char * text) {
  for (size_t i = 0; i < strlen(text)-1; i++) {
    if (text[i] == '(' || text[i] == '\'') {
      if (text[i+1] == ')' || text[i+1] == '\'') {
        return i + 1;
      }
    }
  }
  return strlen(text);
}

}
