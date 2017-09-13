#ifndef KANDINSKY_FRAMEBUFFER_H
#define KANDINSKY_FRAMEBUFFER_H

#include <kandinsky/color.h>
#include <kandinsky/rect.h>

class KDFrameBuffer {
public:
  KDFrameBuffer(KDColor * pixels, KDSize size) : m_pixels(pixels), m_size(size) {}
  void pushRect(KDRect rect, const KDColor * pixels);
  void pushRectUniform(KDRect rect, KDColor color);
  void pullRect(KDRect rect, KDColor * pixels);
  KDRect bounds() {
    return KDRect(KDPointZero, m_size);
  }
private:
  KDColor * pixelAddress(KDPoint p) const;
  KDColor * m_pixels;
  KDSize m_size;
};

#endif
