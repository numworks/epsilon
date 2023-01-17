#ifndef KANDINSKY_ION_CONTEXT_H
#define KANDINSKY_ION_CONTEXT_H

#include <kandinsky/context.h>
#include <omg/global_box.h>

class KDIonContext : public KDContext {
  friend OMG::GlobalBox<KDIonContext>;
public:
  static OMG::GlobalBox<KDIonContext> SharedContext;
  static void Putchar(char c);
  static void Clear(KDPoint newCursorPosition = KDPointZero);
private:
  KDIonContext();
  void pushRect(KDRect rect, const KDColor * pixels) override;
  void pushRectUniform(KDRect rect, KDColor color) override;
  void pullRect(KDRect rect, KDColor * pixels) override;
};

#endif
