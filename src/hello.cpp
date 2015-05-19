extern "C" {
#include "hello.h"
#include <kandinsky.h>
}

#include <poincare.h>

void hello() {
  KDFillRect((KDRect){
      .x = 0,
      .y = 0,
      .width = 240,
      .height = 320},
      0x00);
  /*
  for (int i=0; i <255; i++) {
    KDFillRect((KDRect){.x = (KDCoordinate)i, .y = (KDCoordinate)i, .width = 100, .height = 200}, i);
  }
  KDDrawString("Hello, world", (KDPoint){});
  */

  Number n1 = Number(123);
  Number n2 = Number(45);

  Fraction f = Fraction(&n1, &n2);

  f.layout();
  f.m_frame.origin = (KDPoint){.x = 0, .y = 0};
  f.draw();//(KDPoint){.x = 10, .y = 10});


}
