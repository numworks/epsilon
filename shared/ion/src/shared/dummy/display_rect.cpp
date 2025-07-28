#include <ion/display.h>
namespace Ion {

namespace Display {

void pushRect(KDRect r, const KDColor* pixels) {}
void pushRectUniform(KDRect r, KDColor c) {}
void pullRect(KDRect r, KDColor* pixels) {}
void setScreenshotCallback(void (*callback)()) {}

}  // namespace Display
}  // namespace Ion
