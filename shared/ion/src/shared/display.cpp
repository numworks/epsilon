#include <ion/display.h>

namespace Ion {
namespace Display {

void pushRect(KDRect r, const KDColor* pixels) { pushRectNoFocus(r, pixels); }

void pushRectUniform(KDRect r, KDColor c) { pushRectUniformNoFocus(r, c); }

void pullRect(KDRect r, KDColor* pixels) { pullRectNoFocus(r, pixels); }

}  // namespace Display
}  // namespace Ion
