#pragma once

#include <assert.h>
#include <omg/string.h>

#include <string_view>

namespace Inference {

constexpr static size_t k_groupTitleBufferSize = 20;

// Returns a null-terminated string
constexpr static OMG::String<k_groupTitleBufferSize> GroupTitle(
    int groupIndex, std::string_view groupText, bool letterIndex = false) {
  assert(groupIndex >= 0);
  if (letterIndex) {
    assert(groupIndex <= 'Z' - 'A');
  } else {
    assert(groupIndex <= '9' - '1');
  }
  char digit = (letterIndex ? 'A' : '1') + groupIndex;

  // 1 for the space, 1 for the digit, 1 for the terminating character
  assert(groupText.size() + 3 <= k_groupTitleBufferSize);

  /* Note: we use OMG::String concatenation here because the formatting library
   * (Poincare::Print::CustomPrintf) is not constexpr yet. */
  OMG::String<k_groupTitleBufferSize> result =
      OMG::String<k_groupTitleBufferSize>(groupText);
  result += {' ', digit, '\0'};
  return result;
}

}  // namespace Inference
