#ifndef KANDINSKY_TEXT_H
#define KANDINSKY_TEXT_H

#include <kandinsky/size.h>

class KDText {
public:
  enum class FontSize {
    Small,
    Large
  };
  constexpr static int k_tabCharacterWidth = 4;
  static KDSize stringSize(const char * text, FontSize size = FontSize::Large);
  static KDSize charSize(FontSize size = FontSize::Large);
};

#endif
