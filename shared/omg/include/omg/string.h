#ifndef OMG_STRING_H
#define OMG_STRING_H

#include <stddef.h>

namespace OMG {

// constexpr version of strlen
constexpr static size_t StringLength(const char* string) {
  size_t result = 0;
  while (string[result] != 0) {
    result++;
  }
  return result;
}

/* FIXME : This can be replaced by std::string_view when moving to C++17 */
constexpr static bool StringsAreEqual(const char* s1, const char* s2) {
  return *s1 == *s2 &&
         ((*s1 == '\0' && *s2 == '\0') || StringsAreEqual(s1 + 1, s2 + 1));
}

}  // namespace OMG

#endif
