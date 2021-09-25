#include <ion.h>
#include <kandinsky.h>

namespace Flasher {
namespace Display {

void init() {
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Display::pushRectUniform(KDRect(0,0,Ion::Display::Width,Ion::Display::Height), KDColor::RGB24(0x5e81ac));
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->setOrigin(KDPointZero);
  ctx->setClippingRect(screen);
  ctx->drawString("RECOVERY MODE", KDPoint(10, 10), KDFont::LargeFont, KDColorWhite, KDColor::RGB24(0x5e81ac));
}

}
}
