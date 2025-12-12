#pragma once

#include <assert.h>

/* Equivalent to <algorithm> but for custom algorithms that do not exist in the
 * STL */

namespace OMG {

template <class Iterator>
constexpr bool AllEqual(Iterator begin, Iterator end) {
  assert(begin <= end);
  if (begin == end) {
    // Empty container
    return true;
  }
  Iterator it = begin + 1;
  while (it != end) {
    if (*it != *begin) {
      return false;
    }
    it++;
  }
  return true;
}

}  // namespace OMG
