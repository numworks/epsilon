#include "xnt_loop.h"
#include <assert.h>

CodePoint XNTLoop::XNT(CodePoint defaultCodePoint, bool * shouldRemoveLastCharacter) {
  assert(shouldRemoveLastCharacter != nullptr);
  static constexpr CodePoint XNTCodePoints[] = {'x', 'n', 't', UCodePointGreekSmallLetterTheta};
  int XNTCodePointSize = sizeof(XNTCodePoints) / sizeof(CodePoint);

  if (m_loopIndex == -1) {
    for (int i = 0; i < XNTCodePointSize; i++) {
      if (XNTCodePoints[i] == defaultCodePoint) {
        m_loopIndex = i;
        break;
      }
    }
  } else {
    *shouldRemoveLastCharacter = true;
    m_loopIndex = (m_loopIndex + 1) % XNTCodePointSize;
  }

  return XNTCodePoints[m_loopIndex];
}
