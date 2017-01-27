#ifndef KANDINSKY_TEXT_H
#define KANDINSKY_TEXT_H

#include <kandinsky/size.h>

class KDText {
public:
  enum class FontSize {
    Small,
    Large
  };
  static KDSize stringSize(const char * text, FontSize size = FontSize::Large);
};

#endif
