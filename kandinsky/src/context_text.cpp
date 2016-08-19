#include <kandinsky/context.h>
#include "font.h"

KDColor characterBuffer[BITMAP_FONT_CHARACTER_WIDTH*BITMAP_FONT_CHARACTER_HEIGHT];

void KDContext::drawChar(char character, KDPoint p, uint8_t inverse) {
  for (int j=0; j<BITMAP_FONT_CHARACTER_HEIGHT;j++) {
    for (int i=0; i<BITMAP_FONT_CHARACTER_WIDTH;i++) {
      uint8_t intensity = inverse ?
        bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i] :
        (0xFF-bitmapFont[character-BITMAP_FONT_FIRST_CHARACTER][j][i]);
      KDColor color(intensity * 0x010101);
      characterBuffer[j*BITMAP_FONT_CHARACTER_WIDTH+i] = color;
    }
  }
  fillRectWithPixels(KDRect(p, BITMAP_FONT_CHARACTER_WIDTH, BITMAP_FONT_CHARACTER_HEIGHT),
      characterBuffer,
      characterBuffer);
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
