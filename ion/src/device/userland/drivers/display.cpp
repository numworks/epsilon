#include <ion/display.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Display {

void SVC_ATTRIBUTES pushRect(KDRect r, const KDColor * pixels) {
  SVC(SVC_DISPLAY_PUSH_RECT);
}

void SVC_ATTRIBUTES pushRectUniform(KDRect r, KDColor c) {
  SVC(SVC_DISPLAY_PUSH_RECT_UNIFORM);
}

void SVC_ATTRIBUTES pullRect(KDRect r, KDColor * pixels) {
  SVC(SVC_DISPLAY_PULL_RECT);
}

bool SVC_ATTRIBUTES waitForVBlank() {
  SVC(SVC_DISPLAY_WAIT_FOR_V_BLANK);
}

int SVC_ATTRIBUTES displayUniformTilingSize10(KDColor c) {
  SVC(SVC_DISPLAY_UNIFORM_TILING_SIZE_10);
}

int SVC_ATTRIBUTES displayColoredTilingSize10() {
  SVC(SVC_DISPLAY_COLORED_TILING_SIZE_10);
}

void SVC_ATTRIBUTES POSTPushMulticolor(int rootNumberTiles, int tileSize) {
  SVC(SVC_DISPLAY_POST_PUSH_MULTICOLOR);
}

}
}
