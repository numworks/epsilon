#include <escher/tiled_view.h>
#include <assert.h>

static KDFrameBuffer sCurrentTile;

void tilePushRect(KDRect rect, const KDColor * pixels) {
  KDFramePushRect(&sCurrentTile, rect, pixels);
}
void tilePushRectUniform(KDRect rect, KDColor color) {
  KDFramePushRectUniform(&sCurrentTile, rect, color);
}

void tilePullRect(KDRect rect, KDColor * pixels) {
  KDFramePullRect(&sCurrentTile, rect, pixels);
}

static KDContext sTileContext = {
  {
    &tilePushRect,
    &tilePushRectUniform,
    &tilePullRect
  },
  {0, 0},
  {0, 0, 0, 0}
};

void TiledView::drawRect(KDRect rect) const {
  sCurrentTile.pixels = tile();
  sCurrentTile.size = tileSize();

  sTileContext.clippingRect.origin = KDPointZero; // Useless
  sTileContext.clippingRect.size = sCurrentTile.size;

  KDRect tileRect;
  for (int i=0; i<(rect.width/sCurrentTile.size.width+1); i++) {
    for (int j=0; j<(rect.height/sCurrentTile.size.height+1); j++) {
      tileRect.x = rect.x + i*sCurrentTile.size.width;
      tileRect.y = rect.y + j*sCurrentTile.size.height;
      tileRect.size = sCurrentTile.size;
      //tileRect = KDRectIntersection(tileRect, rect); // Optional
      KDContext * previousContext = KDCurrentContext;
      sTileContext.origin.x = -tileRect.x;
      sTileContext.origin.y = -tileRect.y;

      KDCurrentContext = &sTileContext;
      drawTile(tileRect);
      KDCurrentContext = previousContext;
      KDSize zero = {0,0};

      KDFillRectWithPixels(tileRect, sCurrentTile.pixels, sCurrentTile.pixels);
    }
  }
}
