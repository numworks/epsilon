extern "C" {
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

void ion_app() {
  KDDrawString("Hello, world!", (KDPoint){.x = 10, .y = 10});

  char input[255];

#if 0
  while (1) {
    for (int i =0; i<24;i++) {
      input[i] = ion_getchar();
      input[i+1] = 0;
      KDDrawString(input, (KDPoint){.x = 0, .y = 0});
    }
  }
#endif

  int index = 0;
  while (1) {
    char character = ion_getchar();
    if (character == 'X') {
      input[index] = 0;
      index = 0;
      Expression * e = Expression::parse(input);
      ExpressionLayout * l = e->createLayout(nullptr);
      l->draw(KDPOINT(0,100));

      // FIXME delete l;
      //FIXME delete e;
    } else {
      if (index == 0) {
        KDRect r;
        r.x = 0;
        r.y = 0;
        r.width = 160;
        r.height = 160;
        KDFillRect(r, 0x00);
      }
      input[index++] = character;
      input[index] = 0;
      KDDrawString(input, KDPointZero);
    }
  }
}
