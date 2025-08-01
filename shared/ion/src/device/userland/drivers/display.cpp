#include "display.h"

#include <ion/display.h>

#include <type_traits>

#include "svcall.h"

namespace Ion {
namespace Display {

void SVC_ATTRIBUTES pushRect(KDRect r, const KDColor* pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT)
}

void SVC_ATTRIBUTES pushRectUniform(KDRect r, KDColor c) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT_UNIFORM)
}

void SVC_ATTRIBUTES pullRect(KDRect r, KDColor* pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PULL_RECT)
}

bool SVC_ATTRIBUTES waitForVBlank() {
  SVC_RETURNING_R0(SVC_DISPLAY_WAIT_FOR_V_BLANK, bool)
}

void SVC_ATTRIBUTES POSTPushMulticolor(int rootNumberTiles, int tileSize) {
  SVC_RETURNING_VOID(SVC_DISPLAY_POST_PUSH_MULTICOLOR)
}

static_assert(
    std::is_same_v<decltype(&drawString),
                   void (*)(const char* text, KDPoint point, bool largeFont,
                            KDColor textColor, KDColor backgroundColor)>,
    "Signature of drawString changed");

void drawString(const char* text, KDPoint point, bool largeFont,
                KDColor textColor, KDColor backgroundColor) {
  KDContext* ctx = Ion::Display::Context::SharedContext;
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(Ion::Display::Rect);
  ctx->drawString(text, point,
                  KDGlyph::Style{.glyphColor = textColor,
                                 .backgroundColor = backgroundColor,
                                 .font = largeFont ? KDFont::Size::Large
                                                   : KDFont::Size::Small},
                  255);
}

void setScreenshotCallback(void (*)(void)) {}

}  // namespace Display
}  // namespace Ion
