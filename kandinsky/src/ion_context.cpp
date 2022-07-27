#include <kandinsky/ion_context.h>
#include <ion/display.h>

KDIonContext * KDIonContext::SharedContext() {
  static KDIonContext context;
  return &context;
}

KDIonContext::KDIonContext() :
KDContext(KDPointZero,
    KDRect(0, 0, Ion::Display::Width, Ion::Display::Height))
{
}

void KDIonContext::pushRect(KDRect rect, const KDColor * pixels) {
  Ion::Display::pushRect(rect, pixels);
}

void KDIonContext::pushRectUniform(KDRect rect, KDColor color) {
  Ion::Display::pushRectUniform(rect, color);
}

void KDIonContext::pullRect(KDRect rect, KDColor * pixels) {
  Ion::Display::pullRect(rect, pixels);
}

void KDIonContext::Putchar(char c) {
  static KDPoint cursor = KDPointZero;
  char text[2] = {c, 0};
  if (cursor.y() > Ion::Display::Height - KDFont::GlyphHeight(KDFont::Size::Large)) {
    SharedContext()->pushRectUniform(KDRect(0, 0, Ion::Display::Width, Ion::Display::Height), KDColorWhite);
    cursor = KDPoint(cursor.x(), 0);
  }
  cursor = SharedContext()->drawString(text, cursor);
}
