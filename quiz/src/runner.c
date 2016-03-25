#include "symbols.h"
#include <string.h>
#include <kandinsky.h>
#include <ion.h>

void print(char * message) {
  static int line_y = 0;
  int line_height = KDStringSize("M").height;
  KDDrawString(message, (KDPoint){.x = 0, .y = line_y});
  line_y += line_height;
  if (line_y > SCREEN_HEIGHT) {
    line_y = 0;
    // Clear screen maybe?
  }
}

void ion_app() {
  int i = 0;
  while (quiz_cases[i] != NULL) {
    QuizCase c = quiz_cases[i];
    c();
    print(quiz_case_names[i]);
    i++;
  }
  print("ALL TESTS FINISHED");
  while (1) {
    ion_sleep();
  }
}
