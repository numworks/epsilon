#ifndef ESCHER_CLIPPED_BOARD_H
#define ESCHER_CLIPPED_BOARD_H

#include <escher/text_field.h>

class ClippedBoard {
public:
  ClippedBoard();
  static ClippedBoard * sharedClippedBoard();
  void store(const char * storedText);
  const char * storedText();
  void reset();
private:
  char m_textBuffer[TextField::maxBufferSize()];
};

#endif
