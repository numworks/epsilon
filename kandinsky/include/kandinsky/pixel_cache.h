#ifndef KANDINSKY_PIXEL_CACHE_H
#define KANDINSKY_PIXEL_CACHE_H

#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>

template <unsigned Size>
class KDPixelCache {
public:
  KDPixelCache() : m_rect(KDRectZero) {}

  void save(KDContext * ctx, KDRect rect) {
    assert(rect.width() * rect.height() <= Size);
    ctx->getPixels(rect, m_buffer);
    m_rect = rect;
  }

  void restore(KDContext * ctx) {
    ctx->fillRectWithPixels(m_rect, m_buffer, m_buffer);
    m_rect = KDRectZero;
  }
private:
  KDRect m_rect;
  KDColor m_buffer[Size];
};

#endif
