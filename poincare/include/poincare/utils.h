#ifndef POINCARE_UTILS_H
#define POINCARE_UTILS_H

// Add small constexpr utility methods here
namespace Poincare {

/* FIXME : This can be replaced by std::string_view when moving to C++17 */
constexpr bool strings_are_equal(const char * s1, const char * s2) {
  return *s1 == *s2 && ((*s1 == '\0' && *s2 =='\0') || strings_are_equal(s1 + 1, s2 + 1));
}

constexpr int string_length(const char * string) {
  int result = 0;
  while (string[result] != 0) { result ++; }
  return result;
}

}

#endif