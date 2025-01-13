#include <apps/init.h>
#include <escher/init.h>
#include <ion.h>
#include <poincare/init.h>
#include <poincare/print.h>
#include <poincare/test/helper.h>

#include "quiz.h"
#include "runner_helpers.h"
#include "symbols.h"

void quiz_print(const char* message) { Ion::Console::writeLine(message); }

bool quiz_print_clear() { return Ion::Console::clear(); }

static inline void ion_main_inner(const char* testFilter) {
  int i = 0;
  int time = Ion::Timing::millis();
  int totalCases = 0;

  // First pass to count the number of quiz cases
  while (quiz_cases[i] != NULL) {
#ifndef PLATFORM_DEVICE
    if (testFilter &&
        strstr(quiz_case_names[i], testFilter) != quiz_case_names[i]) {
      i++;
      continue;
    }
#endif
    i++;
    totalCases++;
  }

  // Second pass to test quiz cases
  constexpr int k_bufferSize = 30;
  char buffer[k_bufferSize];
  i = 0;
  int caseIndex = 0;
  while (quiz_cases[i] != NULL) {
#ifndef PLATFORM_DEVICE
    if (testFilter &&
        strstr(quiz_case_names[i], testFilter) != quiz_case_names[i]) {
      i++;
      continue;
    }
#endif
    caseIndex++;
    QuizCase c = quiz_cases[i];
    if (quiz_print_clear()) {
      // Avoid cluttering the display if it can't be cleared
      Poincare::Print::CustomPrintf(buffer, k_bufferSize, "TEST: %i/%i",
                                    caseIndex, totalCases);
      quiz_print(buffer);
    }
    quiz_print(quiz_case_names[i]);
    flushGlobalData();
    c();
    i++;
  }
  flushGlobalData();

  quiz_print_clear();

  // Display test results
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "ALL %i TESTS FINISHED",
                                caseIndex);
  quiz_print(buffer);

  // Display test duration
  time = Ion::Timing::millis() - time;
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "DURATION: %i ms", time);
  quiz_print(buffer);
#ifdef PLATFORM_DEVICE
  while (1) {
    Ion::Timing::msleep(100000);
  }
#endif
}

void ion_main(int argc, const char* const argv[]) {
  // Initialize TreePool::sharedPool and TreeStack::SharedTreeStack
  Poincare::Init();
  Escher::Init();
  Apps::Init();

  const char* testFilter = nullptr;
  sSkipAssertions = false;
#if !PLATFORM_DEVICE
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--filter") == 0 || strcmp(argv[i], "-f") == 0) {
      assert(i + 1 < argc);
      testFilter = argv[i + 1];
    } else if (strcmp(argv[i], "--skip-assertions") == 0 ||
               strcmp(argv[i], "-s") == 0) {
      sSkipAssertions = true;
    }
  }
#endif
  /* s_stackStart must be defined as early as possible to ensure that there
   * cannot be allocated memory pointers before. Otherwise, with MicroPython for
   * example, stack pointer could go backward after initialization and allocated
   * memory pointers could be overlooked during mark procedure. */
  volatile int stackTop;
  Ion::setStackStart((void*)(&stackTop));
  exception_run(ion_main_inner, testFilter);
  Poincare::Shutdown();
}
