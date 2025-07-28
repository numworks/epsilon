#include <ion/display.h>

namespace Ion::Display {

OMG::GlobalBox<Context> Context::SharedContext;

Context::Context() : KDContext(KDPointZero, Ion::Display::Rect) {}

void Context::pushRect(KDRect rect, const KDColor* pixels) {
  Ion::Display::pushRect(rect, pixels);
}

void Context::pushRectUniform(KDRect rect, KDColor color) {
  Ion::Display::pushRectUniform(rect, color);
}

void Context::pullRect(KDRect rect, KDColor* pixels) {
  Ion::Display::pullRect(rect, pixels);
}

static KDPoint s_cursor = KDPointZero;

void Context::Putchar(char c) {
  constexpr KDFont::Size font = KDFont::Size::Large;
  constexpr KDGlyph::Style style = {.font = font};
  char text[2] = {c, 0};
  if (s_cursor.x() > Ion::Display::Width - KDFont::GlyphWidth(font, c)) {
    s_cursor = SharedContext->drawString("\n", s_cursor, style);
  }
  if (s_cursor.y() > Ion::Display::Height - 2 * KDFont::GlyphHeight(font)) {
    Clear(KDPoint(s_cursor.x(), 0));
  }
  s_cursor = SharedContext->drawString(text, s_cursor, style);
}

void Context::Clear(KDPoint newCursorPosition) {
  Context* ctx = SharedContext;
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(Ion::Display::Rect);
  ctx->pushRectUniform(Ion::Display::Rect, KDColorWhite);
  s_cursor = newCursorPosition;
}

}  // namespace Ion::Display
