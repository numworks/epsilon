#ifndef KANDINSKY_ION_CONTEXT_H
#define KANDINSKY_ION_CONTEXT_H

#include <kandinsky/context.h>
#include <ion/display.h>

class KDIonContext : public KDContext {
public:
  static KDIonContext * sharedContext() {
    return &s_context;
  }
private:
  KDIonContext() : KDContext(KDPointZero, KDRect(0, 0, Ion::Display::Width, Ion::Display::Height)) {}
  void pushRect(KDRect rect, const KDColor * pixels) override {
    Ion::Display::pushRect(rect, pixels);
  }
  void pushRectUniform(KDRect rect, KDColor color) override {
    Ion::Display::pushRectUniform(rect, color);
  }
  void pullRect(KDRect rect, KDColor * pixels) override {
    Ion::Display::pullRect(rect, pixels);
  }
  static KDIonContext s_context;
};

#endif
