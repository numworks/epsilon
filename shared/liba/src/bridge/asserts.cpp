#include <assert.h>

#include <cstdlib>
#include <functional>
#include <string>

[[gnu::noinline, noreturn]] static void assert_fail_impl(const char* assertion,
                                                         const char* file,
                                                         unsigned int line) {
  /* afl-tmin distinguishes crashes by exit code. Hash the file:line couple to
   * derive a stable return code per assert. The code is between 1 and 127, so
   * it never collides with the 128+sig codes used for signal-induced crashes.
   */
  std::hash<std::string> hasher;
  std::exit((hasher(std::string(file)) + line) % 127 + 1);
}

#if defined(__linux__)
// Part of the glibc ABI, called by the assert macro
[[noreturn]] void __assert_fail(const char* assertion, const char* file,
                                unsigned int line, const char* function) {
  assert_fail_impl(assertion, file, line);
}
#elif defined(__APPLE__)
// Part of the macOS ABI, called by the assert macro
[[noreturn]] void __assert_rtn(const char* function, const char* file, int line,
                               const char* assertion) {
  assert_fail_impl(assertion, file, static_cast<unsigned int>(line));
}
#else
#warning The assert override is only implemented for linux and macOS for now.
#endif
