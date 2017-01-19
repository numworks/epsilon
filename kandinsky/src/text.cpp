#include <kandinsky/text.h>
#include <string.h>
#include "small_font.h"
#include "large_font.h"

KDSize KDText::stringSize(const char * text, FontSize size) {
  if (text == nullptr) {
    return KDSizeZero;
  }
  if (size == FontSize::Large) {
    return KDSize(BITMAP_LargeFont_CHARACTER_WIDTH*strlen(text), BITMAP_LargeFont_CHARACTER_HEIGHT);
  }
  return KDSize(BITMAP_SmallFont_CHARACTER_WIDTH*strlen(text), BITMAP_SmallFont_CHARACTER_HEIGHT);
}
