#include <kandinsky/text.h>
#include "framebuffer.h"
#include "font.h"

void KDDrawChar(char character, KDPoint p) {
  char * framebuffer = FRAMEBUFFER_ADDRESS;
  for (int j=0; j<BITMAP_FONT_CHARACTER_HEIGHT;j++) {
    for (int i=0; i<BITMAP_FONT_CHARACTER_WIDTH;i++) {
      PIXEL(p.x+i, p.y+j) = bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i];
    }
  }
}

void KDDrawString(char * text, KDPoint p) {
  char * charPointer = text;
  KDPoint position = p;
  while(*charPointer != 0) {
    KDDrawChar(*charPointer, position);
    charPointer++;
    position.x += BITMAP_FONT_CHARACTER_WIDTH;
  }
}
