#include <assert.h>
#include <ion.h>
#include <ion/timing.h>
#include <bootloader/interface/static/interface.h>
#include <bootloader/interface/static/messages.h>

#include <bootloader/computer.h>

namespace Bootloader {

void Interface::drawComputer(KDContext * ctx, int offset) {
  const Image * image = ImageStore::Computer;

  const uint8_t * data;
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

KDCoordinate Interface::computerHeight() {
  return ImageStore::Computer->height();
}

void Interface::drawFlasher() {
  KDContext * ctx = KDIonContext::sharedContext();

  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorWhite);
  drawComputer(ctx, 25);

  KDSize fontSize = KDFont::LargeFont->glyphSize();
  int initPos = (320 - fontSize.width() * strlen(Messages::mainTitle)) / 2;
  ctx->drawString(Messages::mainTitle, KDPoint(initPos, ImageStore::Computer->height() + 25 + 10), KDFont::LargeFont, KDColorBlack, KDColorWhite);

  int y = ImageStore::Computer->height() + 25 + 10 + (KDFont::SmallFont->glyphSize().height() + 10);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::dfuSlotsUpdate)) / 2;
  ctx->drawString(Messages::dfuSlotsUpdate, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
}

void Interface::drawLoading() {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorWhite);
  drawComputer(ctx, 25);
  Ion::Timing::msleep(250);
  KDSize fontSize = KDFont::LargeFont->glyphSize();
  int initPos = (320 - fontSize.width() * strlen(Messages::mainTitle)) / 2;

  for (uint8_t i = 0; i < strlen(Messages::mainTitle); i++) {
    char tmp[2] = {Messages::mainTitle[i], '\0'};
    ctx->drawString(tmp, KDPoint(initPos + i * (fontSize.width()), ImageStore::Computer->height() + 25 + 10), KDFont::LargeFont, KDColorBlack, KDColorWhite);
    Ion::Timing::msleep(50);
  }
  Ion::Timing::msleep(500);
}

}
