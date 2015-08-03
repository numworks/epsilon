extern "C" {
#include "hello.h"
#include <kandinsky.h>
#include <stdlib.h>
#include <ion.h>
}

#include <poincare.h>

void hello() {

  /*char letter = 'Z';
  while (1) {
    letter = getc();
  }
  */

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

  /*

  Number n1 = Number(123);
  Number n2 = Number(45);

  Fraction f = Fraction(&n1, &n2);

  Number n3 = Number(3);
  Power p = Power(&f, &n3);

  p.recursiveLayout();
  p.m_frame.origin = KDPOINT(0, 0);
  p.recursiveDraw();


  char * test = (char *)malloc(10);
  char * bar = (char *)malloc(8);

  free(bar);
  free(test);
  */
/*
  while (1) {
    char character = getc();
    KDDrawChar(character, (KDPoint){.x = 0, .y = 0});
  }
  */


  char input[255];

  int index = 0;
  while (1) {
    char character = ion_getchar();
    if (character == 'X') {
      input[index] = 0;
      index = 0;
      CreateFromString(input);
    } else {
      input[index++] = character;
      input[index] = 0;
      KDDrawString(input, (KDPoint){.x = 0, .y = 0});
    }
  }

  /*
  char * input = "12/34/8787/29292929";
  CreateFromString(input);
*/

}
