#include <ion/display.h>
#include <kandinsky/ion_context.h>

OMG::GlobalBox<KDIonContext> KDIonContext::SharedContext;

KDIonContext::KDIonContext() : KDContext(KDPointZero, Ion::Display::Rect) {}

void KDIonContext::pushRect(KDRect rect, const KDColor* pixels) {
  Ion::Display::pushRect(rect, pixels);
}

void KDIonContext::pushRectUniform(KDRect rect, KDColor color) {
  Ion::Display::pushRectUniform(rect, color);
}

void KDIonContext::pullRect(KDRect rect, KDColor* pixels) {
  Ion::Display::pullRect(rect, pixels);
}

static KDPoint s_cursor = KDPointZero;

void KDIonContext::Putchar(char c) {
  constexpr KDFont::Size font = KDFont::Size::Large;
  constexpr KDGlyph::Style style = {.font = font};
  char text[2] = {c, 0};
  if (s_cursor.x() > Ion::Display::Width - KDFont::GlyphWidth(font)) {
    s_cursor = SharedContext->drawString("\n", s_cursor, style);
  }
  if (s_cursor.y() > Ion::Display::Height - 2 * KDFont::GlyphHeight(font)) {
    Clear(KDPoint(s_cursor.x(), 0));
  }
  s_cursor = SharedContext->drawString(text, s_cursor, style);
}

void KDIonContext::Clear(KDPoint newCursorPosition) {
  KDRect screen(0, 0, Ion::Display::Width, Ion::Display::Height);
  KDIonContext* ctx = SharedContext;
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  ctx->pushRectUniform(screen, KDColorWhite);
  s_cursor = newCursorPosition;
}
