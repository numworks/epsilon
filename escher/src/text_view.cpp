#include <escher/text_view.h>

KDRect ViewFrame(KDPoint origin, const char * text) {
  KDRect r;
  r.origin = origin;
  r.size = KDStringSize(text);
  return r;
}

TextView::TextView(KDPoint origin, const char * text) :
  View(ViewFrame(origin, text)) {
  m_text = text;
}

void TextView::drawRect(KDRect rect) {
  KDPoint zero = {0, 0};
  KDDrawString(m_text, zero, 0);
}
