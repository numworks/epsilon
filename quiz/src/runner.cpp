#include "quiz.h"
#include "symbols.h"
#include <ion.h>
#include <escher/init.h>
#include <poincare/init.h>
#include <poincare/tree_pool.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/print.h>

void quiz_print(const char * message) {
  Ion::Console::writeLine(message);
}

bool quiz_print_clear() {
  return Ion::Console::clear();
}

static inline void ion_main_inner(const char * testFilter) {
  int i = 0;
  int time = Ion::Timing::millis();
  int totalCases = 0;

  // First pass to count the number of quiz cases
  while (quiz_cases[i] != NULL) {
#ifndef PLATFORM_DEVICE
    if (testFilter && strstr(quiz_case_names[i], testFilter) != quiz_case_names[i]) {
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
    if (testFilter && strstr(quiz_case_names[i], testFilter) != quiz_case_names[i]) {
      i++;
      continue;
    }
#endif
    caseIndex++;
    QuizCase c = quiz_cases[i];
    if (quiz_print_clear()) {
      // Avoid cluttering the display if it can't be cleared
      Poincare::Print::CustomPrintf(buffer, k_bufferSize, "TEST: %i/%i", caseIndex, totalCases);
      quiz_print(buffer);
    }
    quiz_print(quiz_case_names[i]);
    int initialPoolSize = Poincare::TreePool::sharedPool()->numberOfNodes();
    quiz_assert(initialPoolSize == 0);
    c();
    int currentPoolSize = Poincare::TreePool::sharedPool()->numberOfNodes();
    quiz_assert(initialPoolSize == currentPoolSize);
    i++;
  }
  quiz_print_clear();

  // Display test results
  Poincare::Print::CustomPrintf(buffer, k_bufferSize, "ALL %i TESTS FINISHED", caseIndex);
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


void ion_main(int argc, const char * const argv[]) {
  Poincare::Init(); // Initialize Poincare::TreePool::sharedPool
  Escher::Init();
  const char * testFilter = nullptr;
  sSkipAssertions = false;
#if !PLATFORM_DEVICE
  for (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "--filter") == 0) {
      assert(i+1 < argc);
      testFilter = argv[i+1];
    } else if (strcmp(argv[i], "--skip-assertions") == 0) {
      sSkipAssertions = true;
    }
  }
  /* s_stackStart must be defined as early as possible to ensure that there
   * cannot be allocated memory pointers before. Otherwise, with MicroPython for
   * example, stack pointer could go backward after initialization and allocated
   * memory pointers could be overlooked during mark procedure. */
  volatile int stackTop;
  Ion::setStackStart((void *)(&stackTop));
#endif
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    ion_main_inner(testFilter);
  } else {
    // There has been a memory allocation problem
#if POINCARE_TREE_LOG
    Poincare::TreePool::sharedPool()->log();
#endif
    quiz_assert(false);
  }
}
