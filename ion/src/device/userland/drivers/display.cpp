#include <drivers/display.h>
#include <drivers/svcall.h>
#include <ion/display.h>
#include <kandinsky/ion_context.h>

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

void drawString(const char* text, KDPoint point, KDGlyph::Style style) {
  KDContext* ctx = KDIonContext::SharedContext;
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(KDRectScreen);
  ctx->drawString(text, point, style, 255);
}

void setScreenshotCallback(void (*)(void)) {}

}  // namespace Display
}  // namespace Ion
