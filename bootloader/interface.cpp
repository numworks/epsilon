
#include <assert.h>
#include <ion.h>

#include <bootloader/interface.h>
#include <bootloader/slot.h>
#include <bootloader/boot.h>

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
  ctx->fillRect(KDRect(0,0,320,240), KDColorBlack);
  drawImage(ctx, ImageStore::Computer, 70);
  drawImage(ctx, ImageStore::Cable, 172);

  ctx->drawString("Slot A:", KDPoint(0, 0),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
  ctx->drawString("Slot B:", KDPoint(0, 13), KDFont::SmallFont, KDColorWhite, KDColorBlack);
  ctx->drawString("Current:", KDPoint(0, 26),  KDFont::SmallFont, KDColorWhite, KDColorBlack);

  if (Boot::mode() == BootMode::SlotA) {
    ctx->drawString("Slot A", KDPoint(63, 26),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
  } else if (Boot::mode() == BootMode::SlotB) {
    ctx->drawString("Slot B", KDPoint(63, 26),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
  }

  Slot slots[2] = {Slot::A(), Slot::B()};

  for(uint8_t i = 0; i < 2; i++) {
    Slot slot = slots[i];

    if (slot.kernelHeader()->isValid() && slot.userlandHeader()->isValid()) {
      if (slot.userlandHeader()->isOmega() && slot.userlandHeader()->isUpsilon()) {
        ctx->drawString("Upsilon", KDPoint(56, i*13),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
        ctx->drawString(slot.userlandHeader()->upsilonVersion(), KDPoint(112, i*13),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
      } else if (slot.userlandHeader()->isOmega()) {
        ctx->drawString("Omega", KDPoint(56, i*13),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
        ctx->drawString(slot.userlandHeader()->omegaVersion(), KDPoint(112, i*13),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
      } else {
        ctx->drawString("Epsilon", KDPoint(56, i*13),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
        ctx->drawString(slot.userlandHeader()->version(), KDPoint(112, i*13),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
      }
      ctx->drawString(slot.kernelHeader()->patchLevel(), KDPoint(168, i*13),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
    } else {
      ctx->drawString("Invalid", KDPoint(56, i*13),  KDFont::SmallFont, KDColorWhite, KDColorBlack);
    }
  }

}

}
