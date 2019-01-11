#include <escher/text_input_helpers.h>
#include <string.h>

namespace TextInputHelpers {

size_t CursorIndexInCommand(const char * text) {
  // TODO LEA
  size_t index = 0;
  while (text[index] != 0) {
    if (text[index] == '\'' &&  text[index+1] == '\'') {
      return index + 1;
    } /* TODO else if (text[index] == Ion::Charset::Empty) {
      return index;
    }*/
    index++;
  }
  return index;
}

}
