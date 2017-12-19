#ifndef KANDINSKY_TEXT_H
#define KANDINSKY_TEXT_H

#include <kandinsky/size.h>
#include "../src/large_font.h"
#include "../src/small_font.h"

class KDText {
public:
  enum class FontSize {
    Small,
    Large
  };
  constexpr static int k_tabCharacterWidth = 4;
  static KDSize stringSize(const char * text, FontSize size = FontSize::Large);
  constexpr static KDSize charSize(FontSize size = FontSize::Large) {
    return size == FontSize::Large ? KDSize(BITMAP_LargeFont_CHARACTER_WIDTH, BITMAP_LargeFont_CHARACTER_HEIGHT) : KDSize(BITMAP_SmallFont_CHARACTER_WIDTH, BITMAP_SmallFont_CHARACTER_HEIGHT);
  }
};

#endif
