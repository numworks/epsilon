#include <ion/contrast.h>
#include <ion/display.h>

namespace Ion {
namespace Display {

void POSTPushMulticolor(int rootNumberTiles, int tileSize) {}

int displayUniformTilingSize10(KDColor c) { return 0; }

int displayColoredTilingSize10() { return 0; }

bool waitForVBlank() { return true; }

void reconfigure(ScreenParameters p) {}

void refreshDisplay() {}

}  // namespace Display
}  // namespace Ion

namespace Ion {
namespace Contrast {

void setScreenContrast(uint8_t vop, uint8_t lightGray, uint8_t darkGray) {}

void persistScreenContrast(uint8_t vop, uint8_t lightGray, uint8_t darkGray) {}

}  // namespace Contrast
}  // namespace Ion
