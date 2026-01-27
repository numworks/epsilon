#pragma once

#include <assert.h>

void quiz_assert(bool condition);
void quiz_print(const char* message);

#define QUIZ_CASE(name)                                                 \
  namespace Quiz {                                                      \
  void case_##name();                                                   \
  static ::Quiz::RegisterCase register_case_##name(#name, case_##name); \
  } /* namespace Quiz */                                                \
  void Quiz::case_##name()

namespace Quiz {

using TestCase = void (*)(void);

struct RegisterCase {
  RegisterCase(const char* name, TestCase function);
};

class Env {
  /* The definitions of the functions in Quiz::Env are project dependant, and
   * must be provided by each project using the Quiz framework. */

  static void Init();
  // Called between each test to reset global data
  static void Flush();
  static void Shutdown();

  friend class Runner;
};

}  // namespace Quiz
