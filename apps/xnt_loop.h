#ifndef APPS_XNT_LOOP
#define APPS_XNT_LOOP

#include <ion/unicode/code_point.h>

class XNTLoop {
public:
  XNTLoop(): m_loopIndex(-1) {}
  void reset() { m_loopIndex = -1; }
  CodePoint XNT(CodePoint defaultCodePoint, bool * shouldRemoveLastCharacter);
private:
  int m_loopIndex;
};

#endif
