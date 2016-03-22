#include <kandinsky/text.h>
#include <kandinsky/pixel.h>
#include <string.h>
#include "font.h"

void KDDrawChar(char character, KDPoint p) {
  for (int j=0; j<BITMAP_FONT_CHARACTER_HEIGHT;j++) {
    for (int i=0; i<BITMAP_FONT_CHARACTER_WIDTH;i++) {
      KDSetPixel(KDPointMake(p.x+i, p.y+j), 0xFF-bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i]);
    }
  }
}

void KDDrawInverseChar(char character, KDPoint p) {
  for (int j=0; j<BITMAP_FONT_CHARACTER_HEIGHT;j++) {
    for (int i=0; i<BITMAP_FONT_CHARACTER_WIDTH;i++) {
      KDSetPixel(KDPointMake(p.x+i, p.y+j), bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i]);
    }
  }
}

void KDDrawString(const char * text, KDPoint p) {
  KDPoint position = p;
  while(*text != 0) {
    KDDrawChar(*text, position);
    text++;
    position.x += BITMAP_FONT_CHARACTER_WIDTH;
  }
}

KDSize KDStringSize(char * text) {
  return (KDSize){
    .width = strlen(text)*BITMAP_FONT_CHARACTER_WIDTH,
    .height = BITMAP_FONT_CHARACTER_HEIGHT
  };
}
