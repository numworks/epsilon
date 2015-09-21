extern "C" {
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

void ion_app() {

  KDDrawString("Hello, world!", (KDPoint){.x = 10, .y = 10});

  char input[255];

  int index = 0;
  while (1) {
    char character = ion_getchar();
    if (character == '.') {
      input[index] = 0;
      index = 0;
      Expression * e = Expression::parse(input);
      ExpressionLayout * l = e->createLayout();
      l->m_frame.origin = KDPOINT(0, 100);
      l->draw();

      // FIXME delete l;
      //FIXME delete e;
    } else {
      if (index == 0) {
        KDFillRect((KDRect){
            .x = 0,
            .y = 0,
            .width = 160,
            .height = 160},
            0x00);
      }
      input[index++] = character;
      input[index] = 0;
      KDDrawString(input, (KDPoint){.x = 40, .y = 40});
    }
  }
}
