#include "quiz.h"
#include "symbols.h"
#include <string.h>
#include <kandinsky.h>
#include <poincare/init.h>
#include <ion.h>
#include <poincare/tree_pool.h>
#include <setjmp.h>

void quiz_print(const char * message) {
#if QUIZ_USE_CONSOLE
  Ion::Console::writeLine(message);
#else
  static int line_y = 0;
  KDContext * ctx = KDIonContext::sharedContext();
  int line_height = KDText::stringSize("M").height();
  ctx->drawString(message, KDPoint(0, line_y));
  line_y += line_height;
  if (line_y > Ion::Display::Height) {
    line_y = 0;
    // Clear screen maybe?
  }
#endif
}

void ion_main(int argc, char * argv[]) {
  // Initialize Poincare::TreePool::sharedPool
  Poincare::init();

  jmp_buf jumpEnvironment;
  Poincare::TreePool::sharedPool()->setJumpEnvironment(&jumpEnvironment);
  int res = setjmp(jumpEnvironment);
  if (res != 0) {
    // There has been a memeory allocation problem
    assert(false);
#if !QUIZ_USE_CONSOLE
    while (1) {
      Ion::msleep(1000);
    }
#else
    return;
#endif
  }
  int i = 0;
  while (quiz_cases[i] != NULL) {
    QuizCase c = quiz_cases[i];
    quiz_print(quiz_case_names[i]);
    c();
    i++;
  }
  quiz_print("ALL TESTS FINISHED");
#if !QUIZ_USE_CONSOLE
  while (1) {
    Ion::msleep(1000);
  }
#endif
}
