#include <ion/display.h>
namespace Ion {

namespace Display {

void pushRectNoFocus(KDRect r, const KDColor* pixels) {}
void pushRectUniformNoFocus(KDRect r, KDColor c) {}
void pullRectNoFocus(KDRect r, KDColor* pixels) {}
void setScreenshotCallback(void (*callback)()) {}

}  // namespace Display
}  // namespace Ion
