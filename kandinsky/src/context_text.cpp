#include <kandinsky/context.h>
#include "small_font.h"
#include "large_font.h"

KDColor smallCharacterBuffer[BITMAP_SmallFont_CHARACTER_WIDTH*BITMAP_SmallFont_CHARACTER_HEIGHT];
KDColor largeCharacterBuffer[BITMAP_LargeFont_CHARACTER_WIDTH*BITMAP_LargeFont_CHARACTER_HEIGHT];

void KDContext::drawChar(char character, KDText::FontSize size, KDPoint p, KDColor textColor, KDColor backgroundColor) {
  int firstCharacter = size == KDText::FontSize::Large ? BITMAP_LargeFont_FIRST_CHARACTER : BITMAP_SmallFont_FIRST_CHARACTER;
  int characterHeight = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_HEIGHT : BITMAP_SmallFont_CHARACTER_HEIGHT;
  int characterWidth = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_WIDTH : BITMAP_SmallFont_CHARACTER_WIDTH;
  KDColor * characterBuffer = size == KDText::FontSize::Large ? largeCharacterBuffer : smallCharacterBuffer;
  for (int j=0; j<characterHeight;j++) {
    for (int i=0; i<characterWidth;i++) {
      uint8_t intensity = 0;
      if (size == KDText::FontSize::Large) {
        intensity = bitmapLargeFont[character-firstCharacter][j][i];
      } else {
       intensity = bitmapSmallFont[character-firstCharacter][j][i];
      }
      KDColor color = KDColor::blend(textColor, backgroundColor, intensity);
      characterBuffer[j*characterWidth+i] = color;
    }
  }
  fillRectWithPixels(KDRect(p, characterWidth, characterHeight),
      characterBuffer,
      characterBuffer);
}

void KDContext::drawString(const char * text, KDText::FontSize size, KDPoint p, KDColor textColor, KDColor backgroundColor) {
  KDPoint position = p;
  int characterWidth = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_WIDTH : BITMAP_SmallFont_CHARACTER_WIDTH;
  KDPoint characterSize(characterWidth, 0);
  while(*text != 0) {
    drawChar(*text, size, position, textColor, backgroundColor);
    text++;
    position = position.translatedBy(characterSize);
  }
}

void KDContext::blendChar(char character, KDText::FontSize size, KDPoint p, KDColor textColor) {
  int firstCharacter = size == KDText::FontSize::Large ? BITMAP_LargeFont_FIRST_CHARACTER : BITMAP_SmallFont_FIRST_CHARACTER;
  int characterHeight = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_HEIGHT : BITMAP_SmallFont_CHARACTER_HEIGHT;
  int characterWidth = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_WIDTH : BITMAP_SmallFont_CHARACTER_WIDTH;
  KDColor * characterBuffer = size == KDText::FontSize::Large ? largeCharacterBuffer : smallCharacterBuffer;

  KDRect absoluteRect = absoluteFillRect(KDRect(p, characterWidth, characterHeight));
  pullRect(absoluteRect, characterBuffer);
  KDCoordinate startingI = m_clippingRect.x() - p.translatedBy(m_origin).x();
  KDCoordinate startingJ = m_clippingRect.y() - p.translatedBy(m_origin).y();
  startingI = startingI > 0 ? startingI : 0;
  startingJ = startingJ > 0 ? startingJ : 0;
  for (KDCoordinate j=0; j<absoluteRect.height(); j++) {
    for (KDCoordinate i=0; i<absoluteRect.width(); i++) {
      KDColor * currentPixelAdress = characterBuffer + i + absoluteRect.width()*j;
      uint8_t intensity = 0;
      if (size == KDText::FontSize::Large) {
        intensity = bitmapLargeFont[character-firstCharacter][j + startingJ][i +startingI];
      } else {
       intensity = bitmapSmallFont[character-firstCharacter][j + startingJ][i +startingI];
      }
      *currentPixelAdress = KDColor::blend(textColor, *currentPixelAdress, intensity);
    }
  }
  pushRect(absoluteRect, characterBuffer);
}

void KDContext::blendString(const char * text, KDText::FontSize size, KDPoint p, KDColor textColor) {
  KDPoint position = p;
  int characterWidth = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_WIDTH : BITMAP_SmallFont_CHARACTER_WIDTH;
  KDPoint characterSize(characterWidth, 0);
  while(*text != 0) {
    blendChar(*text, size, position, textColor);
    text++;
    position = position.translatedBy(characterSize);
  }
}
