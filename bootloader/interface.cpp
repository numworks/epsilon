
#include <assert.h>
#include <ion.h>

#include "interface.h"

#include "computer.h"
#include "cable.h"

namespace Bootloader {

void Interface::drawImage(KDContext* ctx, const Image* image, int offset) {
  const uint8_t* data;
  size_t size;
  size_t pixelBufferSize;
 
  if (image != nullptr) {
    data = image->compressedPixelData();
    size = image->compressedPixelDataSize();
    pixelBufferSize = image->width() * image->height();
  } else {
    return;
  }

  KDColor pixelBuffer[4000];
  assert(pixelBufferSize <= 4000);
  assert(Ion::stackSafe()); // That's a VERY big buffer we're allocating on the stack

  Ion::decompress(
    data,
    reinterpret_cast<uint8_t *>(pixelBuffer),
    size,
    pixelBufferSize * sizeof(KDColor)
  );

  KDRect bounds((320 - image->width()) / 2, offset, image->width(), image->height());

  ctx->fillRectWithPixels(bounds, pixelBuffer, nullptr);
}

void Interface::draw() {
  KDContext * ctx = KDIonContext::sharedContext();
  drawImage(ctx, ImageStore::Computer, 70);
  drawImage(ctx, ImageStore::Cable, 172);
}

}
