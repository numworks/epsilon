#include "quiz.h"
#include "symbols.h"
#include <string.h>
#include <ion.h>
#include <kandinsky.h>
#include <poincare/init.h>
#include <poincare/tree_pool.h>
#include <poincare/exception_checkpoint.h>

void quiz_print(const char * message) {
#if QUIZ_USE_CONSOLE
  Ion::Console::writeLine(message);
#else
  static int line_y = 0;
  KDContext * ctx = KDIonContext::sharedContext();
  const KDFont * font = KDFont::LargeFont;
  int line_height = font->glyphSize().height();
  ctx->drawString(message, KDPoint(0, line_y), font);
  line_y += line_height;
  if (line_y + line_height > Ion::Display::Height) {
    line_y = 0;
    // Clear screen maybe?
  }
#endif
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
#if !QUIZ_USE_CONSOLE
  while (1) {
    Ion::Timing::msleep(1000);
  }
#endif
}

void ion_main(int argc, char * argv[]) {
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
#if !QUIZ_USE_CONSOLE
    while (1) {
      Ion::Timing::msleep(1000);
    }
#endif
  }
}
