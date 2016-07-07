#include <kandinsky/ion_context.h>
extern "C" {
#include <ion.h>
}

KDIonContext * KDIonContext::sharedContext() {
  static KDIonContext context;
  return &context;
}

KDIonContext::KDIonContext() :
KDContext(KDPointZero,
    KDRect(0, 0, ION_SCREEN_WIDTH, ION_SCREEN_HEIGHT))
{
}

void KDIonContext::pushRect(KDRect rect, const KDColor * pixels) {
  ion_screen_push_rect(rect.x(), rect.y(), rect.width(), rect.height(), (const ion_color_t *)pixels);
}

void KDIonContext::pushRectUniform(KDRect rect, KDColor color) {
  ion_screen_push_rect_uniform(rect.x(), rect.y(), rect.width(), rect.height(), color);
}

void KDIonContext::pullRect(KDRect rect, KDColor * pixels) {
  ion_screen_pull_rect(rect.x(), rect.y(), rect.width(), rect.height(), (ion_color_t *)pixels);
}
