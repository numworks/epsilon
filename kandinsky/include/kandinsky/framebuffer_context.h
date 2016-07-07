#ifndef KANDINSKY_FRAMEBUFFER_CONTEXT_H
#define KANDINSKY_FRAMEBUFFER_CONTEXT_H

#include <kandinsky/context.h>
#include <kandinsky/framebuffer.h>

class KDFrameBufferContext : public KDContext {
public:
  KDFrameBufferContext(KDFrameBuffer * frameBuffer);
protected:
  void pushRect(KDRect, const KDColor * pixels) override;
  void pushRectUniform(KDRect rect, KDColor color) override;
  void pullRect(KDRect rect, KDColor * pixels) override;
private:
  KDFrameBuffer * m_frameBuffer;
};

#endif
