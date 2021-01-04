#include <ion/display.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Display {

void SVC_ATTRIBUTES pushRectSVC(KDRect * r, const KDColor * pixels) {
  SVC(SVC_PUSH_RECT);
}

void pushRect(KDRect r, const KDColor * pixels) {
  pushRectSVC(&r, pixels);
}

void SVC_ATTRIBUTES pushRectUniformSVC(KDRect * r, KDColor * c) {
  SVC(SVC_PUSH_RECT_UNIFORM);
}

void pushRectUniform(KDRect r, KDColor c) {
  pushRectUniformSVC(&r, &c);
}

void SVC_ATTRIBUTES pullRectSVC(KDRect * r, KDColor * pixels) {
  SVC(SVC_PULL_RECT);
}

void pullRect(KDRect r, KDColor * pixels) {
  pullRectSVC(&r, pixels);
}

void SVC_ATTRIBUTES POSTPushMulticolorSVC(int * rootNumberTiles, int * tileSize) {
  SVC(SVC_POST_PUSH_MULTICOLOR);
}

void POSTPushMulticolor(int rootNumberTiles, int tileSize) {
  POSTPushMulticolorSVC(&rootNumberTiles, &tileSize);
}

}
}
