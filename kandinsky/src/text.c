#include <kandinsky/text.h>
#include <kandinsky/pixel.h>
#include <string.h>
#include "font.h"

void KDDrawChar(char character, KDPoint p, uint8_t inverse) {
  for (int j=0; j<BITMAP_FONT_CHARACTER_HEIGHT;j++) {
    for (int i=0; i<BITMAP_FONT_CHARACTER_WIDTH;i++) {
      uint8_t intensity = inverse ?
        bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i] :
        (0xFF-bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i]);
      KDSetPixel(KDPointMake(p.x+i, p.y+j), intensity);
    }
  }
}

void KDDrawString(const char * text, KDPoint p, uint8_t inverse) {
  KDPoint position = p;
  while(*text != 0) {
    KDDrawChar(*text, position, inverse);
    text++;
    position.x += BITMAP_FONT_CHARACTER_WIDTH;
  }
}

KDSize KDStringSize(const char * text) {
  return (KDSize){
    .width = strlen(text)*BITMAP_FONT_CHARACTER_WIDTH,
    .height = BITMAP_FONT_CHARACTER_HEIGHT
  };
}
