#pragma once

#include <assert.h>
#include <stddef.h>

#include <span>

namespace OMG {

// constexpr version of strlen
constexpr static size_t StringLength(const char* string) {
  size_t result = 0;
  while (string[result] != 0) {
    result++;
  }
  return result;
}

// Helper methods for the std::span<const char> "string view"
// TODO: add std::string_view to the code base
constexpr static inline size_t StringLength(std::span<const char> string) {
  assert(string.back() == '\0');
  // The end character does not count in the string length
  return string.size() - 1;
}

constexpr static inline std::span<const char> ToSpan(const char* string) {
  return std::span<const char>{string, StringLength(string) + 1};
}

constexpr static inline std::span<const char> ToSpan(const char* string,
                                                     size_t stringLength) {
  assert(StringLength(string) == stringLength);
  return ToSpan(string);
}

/* FIXME : This can be replaced by std::string_view when moving to C++17 */
constexpr static bool StringsAreEqual(const char* s1, const char* s2) {
  return *s1 == *s2 &&
         ((*s1 == '\0' && *s2 == '\0') || StringsAreEqual(s1 + 1, s2 + 1));
}

}  // namespace OMG
