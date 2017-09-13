#ifndef KANDINSKY_FRAMEBUFFER_CONTEXT_H
#define KANDINSKY_FRAMEBUFFER_CONTEXT_H

#include <kandinsky/context.h>
#include <kandinsky/framebuffer.h>

class KDFrameBufferContext : public KDContext {
public:
  KDFrameBufferContext(KDFrameBuffer * frameBuffer) :
    KDContext(KDPointZero, frameBuffer->bounds()),
    m_frameBuffer(frameBuffer) {}
protected:
  void pushRect(KDRect rect, const KDColor * pixels) override {
    m_frameBuffer->pushRect(rect, pixels);
  }
  void pushRectUniform(KDRect rect, KDColor color) override {
    m_frameBuffer->pushRectUniform(rect, color);
  }
  void pullRect(KDRect rect, KDColor * pixels) override {
    m_frameBuffer->pullRect(rect, pixels);
  }
private:
  KDFrameBuffer * m_frameBuffer;
};

#endif
