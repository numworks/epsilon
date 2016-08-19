#include <kandinsky/text.h>
#include <string.h>
#include "font.h"

KDSize KDText::stringSize(const char * text) {
  return KDSize(BITMAP_FONT_CHARACTER_WIDTH*strlen(text), BITMAP_FONT_CHARACTER_HEIGHT);
}
