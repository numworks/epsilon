#include <kandinsky/framebuffer_context.h>

KDFrameBufferContext::KDFrameBufferContext(KDFrameBuffer * frameBuffer) :
  KDContext(KDPointZero, frameBuffer->bounds()),
  m_frameBuffer(frameBuffer)
{
}

void KDFrameBufferContext::pushRect(KDRect rect, const KDColor * pixels) {
  m_frameBuffer->pushRect(rect, pixels);
}

void KDFrameBufferContext::pushRectUniform(KDRect rect, KDColor color) {
  m_frameBuffer->pushRectUniform(rect, color);
}

void KDFrameBufferContext::pullRect(KDRect rect, KDColor * pixels) {
  m_frameBuffer->pullRect(rect, pixels);
}
