#include <kandinsky/context.h>
#include <kandinsky/text.h>
#include "small_font.h"
#include "large_font.h"
#include "font.h"
#include <assert.h>

constexpr int maxGlyphPixelCount = 180;

KDPoint KDContext::drawString(const char * text, KDPoint p, KDText::FontSize size, KDColor textColor, KDColor backgroundColor, int maxLength) {
#define NEW_VERSION 1
#if NEW_VERSION
  const KDFont * font = KDFont::LargeFont;
  KDFont::RenderPalette palette = font->renderPalette(textColor, backgroundColor);

  assert(maxGlyphPixelCount >= font->glyphWidth() * font->glyphHeight());

  const char * c = text;
  while(*c != 0) {
    KDColor glyph[maxGlyphPixelCount];
    font->fetchGlyphForChar(*c, palette, glyph);

    KDRect absoluteRect = absoluteFillRect(KDRect(p, font->glyphWidth(), font->glyphHeight()));
    pushRect(absoluteRect, glyph);
    p = p.translatedBy(KDPoint(font->glyphWidth(), 0));

    c++;
  }

  return KDPointZero;
#else
  return writeString(text, p, size, textColor, backgroundColor, maxLength, false);
#endif
}

#if 0

KDPoint KDContext::blendString(const char * text, KDPoint p, KDText::FontSize size, KDColor textColor) {
  return writeString(text, p, size, textColor, KDColorWhite, -1, true);
}

KDPoint KDContext::writeString(const char * text, KDPoint p, KDText::FontSize size, KDColor textColor, KDColor backgroundColor, int maxLength, bool transparentBackground) {
  KDPoint position = p;
  int characterWidth = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_WIDTH : BITMAP_SmallFont_CHARACTER_WIDTH;
  int characterHeight = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_HEIGHT: BITMAP_SmallFont_CHARACTER_HEIGHT;
  KDPoint characterSize(characterWidth, 0);

  const char * end = text+maxLength;
  while(*text != 0 && text != end) {
    writeChar(*text, position, size, textColor, backgroundColor, transparentBackground);
    if (*text == '\n') {
      position = KDPoint(0, position.y()+characterHeight);
    } else if (*text == '\t') {
      position = position.translatedBy(KDPoint(KDText::k_tabCharacterWidth*characterWidth, 0));
    } else {
      position = position.translatedBy(characterSize);
    }
    text++;
  }
  return position;
}

void KDContext::writeChar(char character, KDPoint p, KDText::FontSize size, KDColor textColor, KDColor backgroundColor, bool transparentBackground) {
  if (character == '\n' || character == '\t') {
    return;
  }
  char firstCharacter = size == KDText::FontSize::Large ? BITMAP_LargeFont_FIRST_CHARACTER : BITMAP_SmallFont_FIRST_CHARACTER;
  int characterHeight = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_HEIGHT : BITMAP_SmallFont_CHARACTER_HEIGHT;
  int characterWidth = size == KDText::FontSize::Large ? BITMAP_LargeFont_CHARACTER_WIDTH : BITMAP_SmallFont_CHARACTER_WIDTH;

  KDColor * characterBuffer = size == KDText::FontSize::Large ? largeCharacterBuffer : smallCharacterBuffer;

  KDRect absoluteRect = absoluteFillRect(KDRect(p, characterWidth, characterHeight));
  if (transparentBackground) {
    pullRect(absoluteRect, characterBuffer);
  }
  KDCoordinate startingI = m_clippingRect.x() - p.translatedBy(m_origin).x();
  KDCoordinate startingJ = m_clippingRect.y() - p.translatedBy(m_origin).y();
  startingI = startingI < 0 ? 0 : startingI;
  startingJ = startingJ < 0 ? 0 : startingJ;

  for (KDCoordinate j=0; j<absoluteRect.height(); j++) {
    for (KDCoordinate i=0; i<absoluteRect.width(); i++) {
      KDColor * currentPixelAdress = characterBuffer + i + absoluteRect.width()*j;         uint8_t intensity = 0;
      if (size == KDText::FontSize::Large) {
        intensity = bitmapLargeFont[(uint8_t)character-(uint8_t)firstCharacter][j + startingJ][i +startingI];
      } else {
       intensity = bitmapSmallFont[(uint8_t)character-(uint8_t)firstCharacter][j + startingJ][i +startingI];
      }
      KDColor backColor = transparentBackground ? *currentPixelAdress : backgroundColor;
      *currentPixelAdress = KDColor::blend(textColor, backColor, intensity);
    }
  }
  pushRect(absoluteRect, characterBuffer);
}
#endif
