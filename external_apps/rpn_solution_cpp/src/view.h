#ifndef VIEW_H
#define VIEW_H

#include "eadkpp.h"
#include "store.h"

class View {
public:
  void drawLayout();
  void drawStore(Store* store);
  void drawInputField(const char* input);

private:
  constexpr static int k_horizontalMargin = 0;
  constexpr static int k_verticalMargin = 20;
  constexpr static int k_characterWidth = 10;
  constexpr static int k_characterHeight = 18;
  constexpr static int k_maxNumberOfCharacters = 256;
  constexpr static EADK::Color k_textColor = EADK::Color(0);
  constexpr static EADK::Color k_backgroundColor = EADK::Color(0xFFFFFF);
};

#endif
