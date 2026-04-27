#include <iomanip>
#include <iostream>

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
