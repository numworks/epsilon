#include "header_titles.h"

namespace Inference {

OMG::String<k_groupTitleBufferSize> GroupTitle(int groupIndex,
                                               bool letterIndex) {
  assert(groupIndex >= 0);
  if (letterIndex) {
    assert(groupIndex <= 'Z' - 'A');
  } else {
    assert(groupIndex <= '9' - '1');
  }
  char digit = (letterIndex ? 'A' : '1') + groupIndex;

  // 1 for the space, 1 for the digit, 1 for the terminating character
  assert(strlen(I18n::translate(I18n::Message::Group)) + 3 <=
         k_groupTitleBufferSize);

  OMG::String<k_groupTitleBufferSize> buffer;
  Poincare::Print::CustomPrintf(buffer.data(), k_groupTitleBufferSize, "%s %c",
                                I18n::translate(I18n::Message::Group), digit);
  return buffer;
}
}  // namespace Inference
