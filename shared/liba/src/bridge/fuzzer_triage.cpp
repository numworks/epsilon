#include <signal.h>
#include <string.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>

#if __has_include(<stacktrace>)
#include <stacktrace>
#define HAS_STACKTRACE 1
#endif

#if !defined(__linux__) && !defined(__APPLE__)
#warning The fuzzer_triage flavor is only useful on linux and macOS for now.
#endif

// Display a crash in CSV format (file,line,reason) to ease parsing
static void print_crash(const char* file, unsigned int line,
                        const char* reason) {
  std::cerr << file << "," << line << "," << std::quoted(reason, '"', '"')
            << "\n";
}

// Strong override of the weak hook in asserts.cpp
extern "C" void fuzzer_on_assert(const char* assertion, const char* file,
                                 unsigned int line) {
  print_crash(file, line, assertion);
}

// noinline preserves the frame count so st[5] consistently points to the caller
[[gnu::noinline]] static void signal_handler(int sig) {
#ifdef HAS_STACKTRACE
  auto st = std::stacktrace::current();
  auto& frame = st[5];
  print_crash(frame.source_file().c_str(), frame.source_line(), strsignal(sig));
#else
  print_crash("", 0, strsignal(sig));
#endif
  /* Use the signal number as exit code so afl-tmin can distinguish crashes. */
  std::exit(sig);
}

[[gnu::constructor]] static void install_signal_handlers() {
  signal(SIGABRT, signal_handler);
  signal(SIGBUS, signal_handler);
  signal(SIGFPE, signal_handler);
  signal(SIGILL, signal_handler);
  signal(SIGSEGV, signal_handler);
}
