#include <escher/clipped_board.h>

static ClippedBoard s_clippedBoard;

ClippedBoard::ClippedBoard()
{
}

ClippedBoard * ClippedBoard::sharedClippedBoard() {
  return &s_clippedBoard;
}

void ClippedBoard::store(const char * storedText) {
  strlcpy(m_textBuffer, storedText, TextField::maxBufferSize());
}

const char * ClippedBoard::storedText() {
  return m_textBuffer;
}

void ClippedBoard::reset() {
  strlcpy(m_textBuffer, "", 1);
}
