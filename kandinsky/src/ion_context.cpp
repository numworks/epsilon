#include <kandinsky/ion_context.h>
#include <ion/display.h>

KDRealIonContext::KDRealIonContext() : KDContext(KDPointZero, KDRect(0, 0, Ion::Display::Width, Ion::Display::Height)) {}
void KDRealIonContext::pushRect(KDRect rect, const KDColor * pixels) {
  Ion::Display::pushRect(rect, pixels);
}
void KDRealIonContext::pushRectUniform(KDRect rect, KDColor color) {
  Ion::Display::pushRectUniform(rect, color);
}
void KDRealIonContext::pullRect(KDRect rect, KDColor * pixels) {
  Ion::Display::pullRect(rect, pixels);
}

KDIonContext * KDIonContext::sharedContext() {
  static KDIonContext context;
  return &context;
}

void KDIonContext::updatePostProcessingEffects() {
  rootContext = &m_realContext;
  if (invertEnabled) {
    invert.setTarget(rootContext);
    rootContext = &invert;
  }
  if (zoomEnabled && !zoomInhibit) {
    zoom.setTarget(rootContext);
    zoom.setTargetArea(KDRect(0,0,320,240));
    zoom.setViewingArea(KDRect(80*(zoomPosition%3),120-60*(zoomPosition/3),160,120));
    rootContext = &zoom;
  }
  if (gammaEnabled) {
    gamma.setTarget(rootContext);
    rootContext = &gamma;
  }
}

KDIonContext::KDIonContext() :
KDContext(KDPointZero,
    KDRect(0, 0, Ion::Display::Width, Ion::Display::Height))
{
}

void KDIonContext::pushRect(KDRect rect, const KDColor * pixels) {
  if (!rootContext) {
    rootContext = &m_realContext;
  }
  rootContext->pushRect(rect, pixels);
}

void KDIonContext::pushRectUniform(KDRect rect, KDColor color) {
  if (!rootContext) {
    rootContext = &m_realContext;
  }
  rootContext->pushRectUniform(rect, color);
}

void KDIonContext::pullRect(KDRect rect, KDColor * pixels) {
  if (!rootContext) {
    rootContext = &m_realContext;
  }
  rootContext->pullRect(rect, pixels);
}

void KDIonContext::putchar(char c) {
  static KDPoint cursor = KDPointZero;
  char text[2] = {c, 0};
  cursor = sharedContext()->drawString(text, cursor);
  if (cursor.y() > Ion::Display::Height) {
    cursor = KDPoint(cursor.x(), 0);
  }
}
