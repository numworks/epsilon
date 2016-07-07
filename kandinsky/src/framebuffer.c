#include <kandinsky/framebuffer.h>
#include <string.h>

static inline KDColor * frameBufferPixelAddress(KDFrameBuffer * frameBuffer, KDCoordinate x, KDCoordinate y) {
    return frameBuffer->pixels + x + y*frameBuffer->size.width;
}

void KDFramePushRect(KDFrameBuffer * frameBuffer, KDRect rect, const KDColor * pixels) {
  const KDColor * line = pixels;
  for (int j=0; j<rect.height; j++) {
    memcpy(frameBufferPixelAddress(frameBuffer, rect.x, rect.y+j),
        line,
        rect.width*sizeof(KDColor));
    line += rect.width;
  }
}

void KDFramePushRectUniform(KDFrameBuffer * frameBuffer, KDRect rect, KDColor color) {
  for (int j=0; j<rect.height; j++) {
    for (int i=0; i<rect.width; i++) {
      *frameBufferPixelAddress(frameBuffer, rect.x+i, rect.y+j) = color;
    }
  }
}

void KDFramePullRect(KDFrameBuffer * frameBuffer, KDRect rect, KDColor * pixels) {
  KDColor * line = pixels;
  for (int j=0; j<rect.height; j++) {
    memcpy(line,
        frameBufferPixelAddress(frameBuffer, rect.x, rect.y+j),
        rect.width*sizeof(KDColor));
    line += rect.width;
  }
}
