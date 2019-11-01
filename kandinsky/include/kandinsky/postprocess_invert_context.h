#ifndef KANDINSKY_POSTPROCESS_INVERT_CONTEXT_H
#define KANDINSKY_POSTPROCESS_INVERT_CONTEXT_H

#include <kandinsky/postprocess_context.h>

class KDPostProcessInvertContext : public KDPostProcessContext {
public:
  KDPostProcessInvertContext() = default;
private:
  void pushRect(KDRect rect, const KDColor * pixels) override;
  void pushRectUniform(KDRect rect, KDColor color) override;
  void pullRect(KDRect rect, KDColor * pixels) override;
};

#endif
