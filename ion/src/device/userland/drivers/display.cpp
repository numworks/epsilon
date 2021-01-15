#include <ion/display.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Display {

void SVC_ATTRIBUTES pushRectSVC(KDRect * r, const KDColor * pixels) {
  SVC(SVC_DISPLAY_PUSH_RECT);
}

void pushRect(KDRect r, const KDColor * pixels) {
  pushRectSVC(&r, pixels);
}

void SVC_ATTRIBUTES pushRectUniformSVC(KDRect * r, KDColor * c) {
  SVC(SVC_DISPLAY_PUSH_RECT_UNIFORM);
}

void pushRectUniform(KDRect r, KDColor c) {
  pushRectUniformSVC(&r, &c);
}

void SVC_ATTRIBUTES pullRectSVC(KDRect * r, KDColor * pixels) {
  SVC(SVC_DISPLAY_PULL_RECT);
}

void pullRect(KDRect r, KDColor * pixels) {
  pullRectSVC(&r, pixels);
}

void SVC_ATTRIBUTES waitForVBlankSVC(bool * res) {
  SVC(SVC_DISPLAY_WAIT_FOR_V_BLANK);
}

bool waitForVBlank() {
  bool res;
  waitForVBlankSVC(&res);
  return res;
}

void SVC_ATTRIBUTES displayUniformTilingSize10SVC(KDColor * c, int * res) {
  SVC(SVC_DISPLAY_UNIFORM_TILING_SIZE_10);
}

int displayUniformTilingSize10(KDColor c) {
  int res;
  displayUniformTilingSize10SVC(&c, &res);
  return res;
}

void SVC_ATTRIBUTES displayColoredTilingSize10SVC(int * res) {
  SVC(SVC_DISPLAY_COLORED_TILING_SIZE_10);
}

int displayColoredTilingSize10() {
  int res;
  displayColoredTilingSize10SVC(&res);
  return res;
}

void SVC_ATTRIBUTES POSTPushMulticolorSVC(int * rootNumberTiles, int * tileSize) {
  SVC(SVC_DISPLAY_POST_PUSH_MULTICOLOR);
}

void POSTPushMulticolor(int rootNumberTiles, int tileSize) {
  POSTPushMulticolorSVC(&rootNumberTiles, &tileSize);
}

}
}
