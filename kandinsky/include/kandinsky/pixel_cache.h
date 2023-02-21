#ifndef KANDINSKY_PIXEL_CACHE_H
#define KANDINSKY_PIXEL_CACHE_H

#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>

class KDAbstractPixelCache {
 public:
  KDAbstractPixelCache() : m_rect(KDRectZero) {}

  void save(KDContext* ctx, KDRect rect) {
    ctx->getPixels(rect, buffer());
    m_rect = rect;
  }

  void restore(KDContext* ctx) {
    ctx->fillRectWithPixels(m_rect, buffer(), buffer());
    m_rect = KDRectZero;
  }

 private:
  virtual KDColor* buffer() = 0;
  KDRect m_rect;
};

template <unsigned Size>
class KDPixelCache : public KDAbstractPixelCache {
 public:
  using KDAbstractPixelCache::KDAbstractPixelCache;

 private:
  KDColor* buffer() override { return m_buffer; }
  KDColor m_buffer[Size];
};

#endif
