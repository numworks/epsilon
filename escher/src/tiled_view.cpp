#include <escher/tiled_view.h>
#include <assert.h>

void TiledView::drawRect(KDContext * ctx, KDRect rect) const {
  KDColor * pixels = tile();
  KDSize size = tileSize();
  KDFrameBuffer tileBuffer(pixels, size);
  KDFrameBufferContext tileContext = KDFrameBufferContext(&tileBuffer);

  for (int i=0; i<(rect.width()/size.width()+1); i++) {
    for (int j=0; j<(rect.height()/size.height()+1); j++) {
      KDRect tileRect(
          rect.x() + i*size.width(),
          rect.y() + j*size.height(),
          size.width(), size.height()
          );
      //tileRect = KDRectIntersection(tileRect, rect); // Optional
      KDPoint origin = tileRect.origin().opposite();
      tileContext.setOrigin(origin);

      drawTile(&tileContext, tileRect);

      ctx->fillRectWithPixels(tileRect, pixels, pixels);
    }
  }
}
