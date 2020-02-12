#include "quiz.h"
#include "symbols.h"
#include <string.h>
#include <ion.h>
#include <kandinsky.h>
#include <poincare/init.h>
#include <poincare/tree_pool.h>
#include <poincare/exception_checkpoint.h>

void quiz_print(const char * message) {
  Ion::Console::writeLine(message);
}

static inline void ion_main_inner() {
  int i = 0;
  while (quiz_cases[i] != NULL) {
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
#ifdef PLATFORM_DEVICE
  while (1) {
    Ion::Timing::msleep(100000);
  }
#endif
}


void ion_main(int argc, const char * const argv[]) {
  // Initialize the backlight
  Ion::Backlight::init();
  // Initialize Poincare::TreePool::sharedPool
  Poincare::Init();

  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    ion_main_inner();
  } else {
    // There has been a memory allocation problem
#if POINCARE_TREE_LOG
    Poincare::TreePool::sharedPool()->log();
#endif
    quiz_assert(false);
  }
}
