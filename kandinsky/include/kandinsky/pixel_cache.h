#ifndef KANDINSKY_PIXEL_CACHE_H
#define KANDINSKY_PIXEL_CACHE_H

#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>
#include "measuring_context.h"

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

  /* Warning : the provided function is evaluated twice, the first time to
   * compute the bounding rect of the actual drawings and the second time will
   * real drawing calls. */
  template<class T> void saveAndDraw(KDContext * ctx, T function) {
    KDMeasuringContext measuringContext(*ctx);
    function(&measuringContext);
    save(ctx, measuringContext.writtenRect());
    function(ctx);
  }
private:
  KDRect m_rect;
  KDColor m_buffer[Size];
};

#endif
