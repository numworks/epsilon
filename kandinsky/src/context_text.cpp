#include <kandinsky/context.h>
#include <string.h>
#include "font.h"

void KDContext::drawChar(char character, KDPoint p, uint8_t inverse) {
  for (int j=0; j<BITMAP_FONT_CHARACTER_HEIGHT;j++) {
    for (int i=0; i<BITMAP_FONT_CHARACTER_WIDTH;i++) {
      uint8_t intensity = inverse ?
        bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i] :
        (0xFF-bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i]);
      KDPoint offset = KDPoint(i,j);
      setPixel(p.translatedBy(offset), intensity);
    }
  }
}

void KDContext::drawString(const char * text, KDPoint p, uint8_t inverse) {
  KDPoint position = p;
  KDPoint characterSize(BITMAP_FONT_CHARACTER_WIDTH, 0);
  while(*text != 0) {
    drawChar(*text, position, inverse);
    text++;
    position = position.translatedBy(characterSize);
  }
}

KDSize KDContext::stringSize(const char * text) {
  return KDSize(BITMAP_FONT_CHARACTER_WIDTH*strlen(text), BITMAP_FONT_CHARACTER_HEIGHT);
}
