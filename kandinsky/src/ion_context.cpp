#include <kandinsky/ion_context.h>
#include <ion.h>

KDIonContext * KDIonContext::sharedContext() {
  static KDIonContext context;
  return &context;
}

KDIonContext::KDIonContext() :
KDContext(KDPointZero,
    KDRect(0, 0, Ion::Display::Width, Ion::Display::Height))
{
}

void KDIonContext::pushRect(KDRect rect, const KDColor * pixels) {
  Ion::Display::pushRect(rect, pixels);
}

void KDIonContext::pushRectUniform(KDRect rect, KDColor color) {
  Ion::Display::pushRectUniform(rect, color);
}

void KDIonContext::pullRect(KDRect rect, KDColor * pixels) {
  Ion::Display::pullRect(rect, pixels);
}
