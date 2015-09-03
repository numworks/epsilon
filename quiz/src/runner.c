#include "symbols.h"
#include <string.h>

void hello() {
  int i = 0;
  while (quiz_cases[i] != NULL) {
    QuizCase c = quiz_cases[i];
    c();
    i++;
  }
}
