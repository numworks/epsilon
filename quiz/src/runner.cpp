#include "quiz.h"
#include "symbols.h"
#include <ion.h>
#include <poincare/init.h>
#include <poincare/tree_pool.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/print.h>

void quiz_print(const char * message) {
  Ion::Console::writeLine(message);
}

static inline void ion_main_inner(const char * testFilter) {
  int i = 0;
  int time = Ion::Timing::millis();
  while (quiz_cases[i] != NULL) {
#ifdef PLATFORM_DEVICE
    if (testFilter && strstr(quiz_case_names[i], testFilter) != quiz_case_names[i]) {
      i++;
      continue;
    }
#endif
    QuizCase c = quiz_cases[i];
    quiz_print(quiz_case_names[i]);
    int initialPoolSize = Poincare::TreePool::sharedPool()->numberOfNodes();
    quiz_assert(initialPoolSize == 0);
    c();
    int currentPoolSize = Poincare::TreePool::sharedPool()->numberOfNodes();
    quiz_assert(initialPoolSize == currentPoolSize);
    i++;
  }
  quiz_print("ALL TESTS FINISHED");
  time = Ion::Timing::millis() - time;
  char timeString[30];
  Poincare::Print::customPrintf(timeString, 30, "DURATION: %i ms", time);
  quiz_print(timeString);
#ifdef PLATFORM_DEVICE
  while (1) {
    Ion::Timing::msleep(100000);
  }
#endif
}


void ion_main(int argc, const char * const argv[]) {
  Poincare::Init(); // Initialize Poincare::TreePool::sharedPool
#if !PLATFORM_DEVICE
  /* s_stackStart must be defined as early as possible to ensure that there
   * cannot be allocated memory pointers before. Otherwise, with MicroPython for
   * example, stack pointer could go backward after initialization and allocated
   * memory pointers could be overlooked during mark procedure. */
  volatile int stackTop;
  Ion::setStackStart((void *)(&stackTop));
#endif

  const char * testFilter = argv[1][0] != '-' ? argv[1] : nullptr;
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
