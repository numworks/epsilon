#include <drivers/display.h>
#include <drivers/svcall.h>
#include <kandinsky/ion_context.h>
#include <ion/display.h>

namespace Ion {
namespace Display {

void SVC_ATTRIBUTES pushRect(KDRect r, const KDColor * pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT)
}

void SVC_ATTRIBUTES pushRectUniform(KDRect r, KDColor c) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PUSH_RECT_UNIFORM)
}

void SVC_ATTRIBUTES pullRect(KDRect r, KDColor * pixels) {
  SVC_RETURNING_VOID(SVC_DISPLAY_PULL_RECT)
}

bool SVC_ATTRIBUTES waitForVBlank() {
  SVC_RETURNING_R0(SVC_DISPLAY_WAIT_FOR_V_BLANK, bool)
}

void SVC_ATTRIBUTES POSTPushMulticolor(int rootNumberTiles, int tileSize) {
  SVC_RETURNING_VOID(SVC_DISPLAY_POST_PUSH_MULTICOLOR)
}

void drawString(const char * text, KDPoint point, bool largeFont, KDColor textColor, KDColor backgroundColor) {
  KDContext * ctx = KDIonContext::SharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(KDRectScreen);
  ctx->drawString(text, point, largeFont ? KDFont::Size::Large : KDFont::Size::Small, textColor, backgroundColor, 255);
}

void saveScreenshot() {}

void copyScreenshot() {}

}
}
