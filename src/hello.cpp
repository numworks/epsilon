extern "C" {
#include "hello.h"
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

void hello() {

  KDDrawString("Hello, world!", (KDPoint){.x = x, .y = 10});

  char input[255];

  int index = 0;
  while (1) {
    char character = ion_getchar();
    if (character == '.') {
      input[index] = 0;
      index = 0;
      Expression * e = Expression::parse(input);
      e->recursiveLayout();
      e->m_frame.origin = KDPOINT(0, 100);
      e->recursiveDraw();
      //FIXME delete e;
    } else {
      if (index == 0) {
        KDFillRect((KDRect){
            .x = 0,
            .y = 0,
            .width = 320,
            .height = 240},
            0x7F);
      }
      input[index++] = character;
      input[index] = 0;
      KDDrawString(input, (KDPoint){.x = 40, .y = 40});
    }
  }
}
