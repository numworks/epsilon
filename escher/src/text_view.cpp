#include <escher/text_view.h>

/*
KDRect ViewFrame(const char * text) {
  KDRect r;
  r.origin = origin;
  r.size = KDStringSize(text);
  return r;
}
*/

TextView::TextView(const char * text) :
  ChildlessView() {
  m_text = text;
}

//TODO: implement "setFrame"

void TextView::drawRect(KDRect rect) {
  KDPoint zero = {0, 0};
  KDDrawString(m_text, zero, 0);
}
