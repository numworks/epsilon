#include <assert.h>
#include <stdio.h>

#include <cstdlib>
#include <functional>
#include <string>

/* Hook called from __assert_fail before exiting. The fuzzer_triage flavor
 * provides a strong override that prints a CSV line to stderr; otherwise this
 * default replicates glibc's assertion message. */
extern "C" [[gnu::weak]] void fuzzer_on_assert(const char* assertion,
                                               const char* file,
                                               unsigned int line) {
  fprintf(stderr, "Assertion `%s' failed at %s:%u\n", assertion, file, line);
}

[[gnu::noinline, noreturn]] static void assert_fail_impl(const char* assertion,
                                                         const char* file,
                                                         unsigned int line) {
  fuzzer_on_assert(assertion, file, line);
  /* afl-tmin distinguishes crashes by exit code. Hash the file:line couple to
   * derive a stable return code per assert. The code is between 1 and 127, so
   * it never collides with the 128+sig codes used for signal-induced crashes.
   */
  std::hash<std::string> hasher;
  std::exit((hasher(std::string(file)) + line) % 127 + 1);
}

#if defined(__linux__)
// Part of the glibc ABI, called by the assert macro
void __assert_fail(const char* assertion, const char* file, unsigned int line,
                   const char*) {
  assert_fail_impl(assertion, file, line);
}
#elif defined(__APPLE__)
// Part of the macOS ABI, called by the assert macro
void __assert_rtn(const char*, const char* file, int line,
                  const char* assertion) {
  assert_fail_impl(assertion, file, static_cast<unsigned int>(line));
}
#else
#warning The assert override is only implemented for linux and macOS for now.
#endif
