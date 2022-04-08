
#include <assert.h>
#include <ion.h>
#include <ion/timing.h>

#include <bootloader/interface.h>
#include <bootloader/messages.h>
#include <bootloader/slot.h>
#include <bootloader/boot.h>

#include "computer.h"

namespace Bootloader {

void Interface::drawImage(KDContext * ctx, const Image * image, int offset) {
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

void Interface::drawHeader() {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorWhite);
  drawImage(ctx, ImageStore::Computer, 25);
  KDSize fontSize = KDFont::LargeFont->glyphSize();
  int initPos = (320 - fontSize.width() * strlen(Messages::mainTitle)) / 2;
  ctx->drawString(Messages::mainTitle, KDPoint(initPos, ImageStore::Computer->height() + fontSize.height() + 10), KDFont::LargeFont, KDColorBlack, KDColorWhite);
}

void Interface::drawMenu() {
  // Get the context
  KDContext * ctx = KDIonContext::sharedContext();
  // Clear the screen
  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorWhite);
  // Draw the image
  drawImage(ctx, ImageStore::Computer, 25);
  // Get the font size
  KDSize largeSize = KDFont::LargeFont->glyphSize();
  KDSize smallSize = KDFont::SmallFont->glyphSize();
  // Draw the title
  int initPos = (320 - largeSize.width() * strlen(Messages::mainMenuTitle)) / 2;
  ctx->drawString(Messages::mainMenuTitle, KDPoint(initPos, ImageStore::Computer->height() + largeSize.height() + 10), KDFont::LargeFont, KDColorBlack, KDColorWhite);

  // Initialize the slot list
  Slot slots[3] = {Slot::A(), Slot::Khi(), Slot::B()};
  char indexes[3] = {'1', '2', '3'};

  // Set the khi slot index, improve this when Khi will have a dedicated header
  const uint8_t khiIndex = 2 - 1;

  // Get the start y position
  int y = ImageStore::Computer->height() + (largeSize.height() + 10) + (smallSize.height() + 10);

  // Iterate over the slot list
  for (uint8_t i = 0; i < sizeof(indexes) / sizeof(indexes[0]); i++) {
    // Get the slot from the list
    Slot slot = slots[i];
    // Get the "X - " string
    char converted[] = {indexes[i], ' ', '-', ' ', '\0'};
    // Setup the margin
    int x = 10;
    // If the slot is valid, draw the slot
    if (slot.kernelHeader()->isValid() && slot.userlandHeader()->isValid()) {
      // Draw the slot number
      ctx->drawString(converted, KDPoint(x, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
      // Increment the x position
      x += strlen(converted) * smallSize.width();
      // Draw the slot version
      ctx->drawString(slot.userlandHeader()->version(), KDPoint(x, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);

      // Get if the commit (patchLevel) isn't empty
      if (slot.kernelHeader()->patchLevel()[0] != '\0') {
        // Increment the x position
        x += strlen(slot.userlandHeader()->version()) * smallSize.width() + smallSize.width() * 2;
        // Draw the slot commit
        ctx->drawString(slot.kernelHeader()->patchLevel(), KDPoint(x, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
        // Increment the x position
        x += strlen(slot.kernelHeader()->patchLevel()) * smallSize.width() + smallSize.width();
      } else {
        // Increment the x position
        x += strlen(slot.userlandHeader()->version()) * smallSize.width() + smallSize.width();
      }

      const char * OSName = "";
      const char * OSVersion = "";
      // If the slot is Upsilon, draw the slot name
      if (slot.userlandHeader()->isOmega() && slot.userlandHeader()->isUpsilon()) {
        // Set the OS name
        OSName = Messages::upsilon;
        // Set the OS version
        OSVersion = slot.userlandHeader()->upsilonVersion();
      } else if (slot.userlandHeader()->isOmega()) {
        // Get if slot is Khi
        bool isKhi = (i == khiIndex);
        // If the slot is Khi, draw the slot name (Khi)
        if (isKhi) {
          // Set the OS name
          OSName = Messages::khi;
        } else {
          // Set the OS name
          OSName = Messages::omega;
        }
        // Set the OS version
        OSVersion = slot.userlandHeader()->omegaVersion();
      } else {
        // Set the OS name
        OSName = Messages::epsilon;
      }
      // Draw the OS name
      ctx->drawString(OSName, KDPoint(x, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
      // Increment the x position
      x += strlen(OSName) * smallSize.width() + smallSize.width();
      // Draw the OS version
      ctx->drawString(OSVersion, KDPoint(x, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
    // Else, the slot is invalid
    } else {
      ctx->drawString(Messages::invalidSlot, KDPoint(10, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
    }
    // Increment the y position
    y += smallSize.height() + 10;
  }

  // Draw the DFU message
  ctx->drawString(Messages::dfuText, KDPoint(10, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);

  // Increment the y position
  y += smallSize.height() + 10;
  // Draw the about message
  ctx->drawString(Messages::aboutText, KDPoint(10, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);

  // Reboot is disabled for now
  // y += smallSize.height() + 10;
  // ctx->drawString(Messages::rebootText, KDPoint(10, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);

  // Draw the footer
  initPos = (320 - smallSize.width() * strlen(Messages::mainTitle)) / 2;
  ctx->drawString(Messages::mainTitle, KDPoint(initPos, 240 - smallSize.height() - 10), KDFont::SmallFont, KDColorBlack, KDColorWhite);
}

void Interface::drawFlasher() {
  Interface::drawHeader();
  KDContext * ctx = KDIonContext::sharedContext();
  int y = ImageStore::Computer->height() + (KDFont::LargeFont->glyphSize().height() + 10) + (KDFont::SmallFont->glyphSize().height() + 10);
  int initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::dfuSubtitle)) / 2;
  ctx->drawString(Messages::dfuSubtitle, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
}

void Interface::drawLoading() {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorWhite);
  drawImage(ctx, ImageStore::Computer, 25);
  Ion::Timing::msleep(250);
  KDSize fontSize = KDFont::LargeFont->glyphSize();
  int initPos = (320 - fontSize.width() * strlen(Messages::mainTitle)) / 2;
  for (uint8_t i = 0; i < strlen(Messages::mainTitle); i++) {
    char tmp[2] = {Messages::mainTitle[i], '\0'};
    ctx->drawString(tmp, KDPoint(initPos + i * (fontSize.width()), ImageStore::Computer->height() + fontSize.height() + 10), KDFont::LargeFont, KDColorBlack, KDColorWhite);
    Ion::Timing::msleep(50);
  }
  Ion::Timing::msleep(500);
}

void Interface::drawAbout() {
  drawHeader();
  // Create the list of about messages
  // TODO: Move it to messages.h
  char aboutMessages[][38] = {
    "This is the bootloader of",
    "the Upsilon Calculator.",
    "It is used to install",
    "and select the OS",
    "to boot."
  };
  // Get the context
  KDContext * ctx = KDIonContext::sharedContext();
  // Get the start Y position
  KDSize largeSize = KDFont::LargeFont->glyphSize();
  KDSize smallSize = KDFont::SmallFont->glyphSize();
  int y = ImageStore::Computer->height() + (largeSize.height() + 10) + (smallSize.height() + 10);
  // Iterate over the list and draw each message
  for (uint8_t i = 0; i < sizeof(aboutMessages) / sizeof(aboutMessages[0]); i++) {
    // Get the message
    char * actualMessage = aboutMessages[i];
    // Get the start X position
    int initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(actualMessage)) / 2;
    // Draw the message
    ctx->drawString(actualMessage, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
    // Increment the Y position
    y += smallSize.height() + 10;
  }

  ctx->drawString(Messages::bootloaderVersion, KDPoint((320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::bootloaderVersion)) / 2, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);

}

void Interface::drawCrash(const char * error) {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorWhite);
  drawImage(ctx, ImageStore::Computer, 25);
  KDSize fontSize = KDFont::LargeFont->glyphSize();
  int initPos = (320 - fontSize.width() * strlen(Messages::crashTitle)) / 2;
  ctx->drawString(Messages::crashTitle, KDPoint(initPos, ImageStore::Computer->height() + fontSize.height() + 10), KDFont::LargeFont, KDColorBlack, KDColorWhite);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(error)) / 2;
  ctx->drawString(error, KDPoint(initPos, ImageStore::Computer->height() + fontSize.height() + 10 + KDFont::SmallFont->glyphSize().height() + 10), KDFont::SmallFont, KDColorBlack, KDColorWhite);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::crashText1)) / 2;
  ctx->drawString(Messages::crashText1, KDPoint(initPos, ImageStore::Computer->height() + fontSize.height() + 10 + KDFont::SmallFont->glyphSize().height() + 10 + KDFont::SmallFont->glyphSize().height() + 10 + 20), KDFont::SmallFont, KDColorBlack, KDColorWhite);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::crashText2)) / 2;
  ctx->drawString(Messages::crashText2, KDPoint(initPos, ImageStore::Computer->height() + fontSize.height() + 10 + KDFont::SmallFont->glyphSize().height() + 10 + KDFont::SmallFont->glyphSize().height() + 10 + 20 + KDFont::SmallFont->glyphSize().height() + 10), KDFont::SmallFont, KDColorBlack, KDColorWhite);
}

void Interface::drawRecovery() {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorWhite);
  drawImage(ctx, ImageStore::Computer, 25);
  KDSize fontSize = KDFont::LargeFont->glyphSize();
  int initPos = (320 - fontSize.width() * strlen(Messages::recoveryTitle)) / 2;
  int y = ImageStore::Computer->height() + fontSize.height() + 5;
  ctx->drawString(Messages::recoveryTitle, KDPoint(initPos, y), KDFont::LargeFont, KDColorBlack, KDColorWhite);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::recoveryText1)) / 2;
  y += fontSize.height() + 5;
  ctx->drawString(Messages::recoveryText1, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::recoveryText2)) / 2;
  y += fontSize.height() + 5;
  ctx->drawString(Messages::recoveryText2, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::recoveryText3)) / 2;
  y += fontSize.height() + 5;
  ctx->drawString(Messages::recoveryText3, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::recoveryText4)) / 2;
  y += fontSize.height() + 5;
  ctx->drawString(Messages::recoveryText4, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::recoveryText5)) / 2;
  y += fontSize.height() + 5;
  ctx->drawString(Messages::recoveryText5, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
}

void Interface::drawInstallerSelection() {
  // Get the context
  KDContext * ctx = KDIonContext::sharedContext();
  // Clear the screen
  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorWhite);
  // Draw the image
  drawImage(ctx, ImageStore::Computer, 25);
  // Get the font size
  KDSize largeSize = KDFont::LargeFont->glyphSize();
  KDSize smallSize = KDFont::SmallFont->glyphSize();
  // Get the start x position
  int initPos = (320 - largeSize.width() * strlen(Messages::installerSelectionTitle)) / 2;
  // Get the start y position
  int y = ImageStore::Computer->height() + largeSize.height() + 10;
  // Draw the title
  ctx->drawString(Messages::installerSelectionTitle, KDPoint(initPos, y), KDFont::LargeFont, KDColorBlack, KDColorWhite);
  // Increment the y position
  y += largeSize.height() + 5;
  // Get the y position of the subtitle
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::installerText1)) / 2;
  // Draw the subtitle
  ctx->drawString(Messages::installerText1, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
  // Increment the y position
  y += smallSize.height() + 10;
  // Set the start x position
  initPos = 10;
  // Draw the first button (slot flash)
  ctx->drawString(Messages::installerText2, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
  // Increment the y position
  y += smallSize.height() + 10;
  // Draw the second button (bootloader flash)
  ctx->drawString(Messages::installerText3, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
}

void Interface::drawBLUpdate() {
  Interface::drawHeader();
  KDContext * ctx = KDIonContext::sharedContext();
  int y = ImageStore::Computer->height() + (KDFont::LargeFont->glyphSize().height() + 10) + (KDFont::SmallFont->glyphSize().height() + 10);
  int initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::bootloaderSubtitle)) / 2;
  ctx->drawString(Messages::bootloaderSubtitle, KDPoint(initPos, y), KDFont::SmallFont, KDColorBlack, KDColorWhite);
}

void Interface::drawEpsilonAdvertisement() {
  KDContext * ctx = KDIonContext::sharedContext();
  ctx->fillRect(KDRect(0, 0, 320, 240), KDColorRed);
  drawImage(ctx, ImageStore::Computer, 25);
  KDSize fontSize = KDFont::LargeFont->glyphSize();
  int initPos = (320 - fontSize.width() * strlen(Messages::epsilonWarningTitle)) / 2;
  int y = ImageStore::Computer->height() + fontSize.height() + 15;
  ctx->drawString(Messages::epsilonWarningTitle, KDPoint(initPos, y), KDFont::LargeFont, KDColorWhite, KDColorRed);
  initPos = (320 - fontSize.width() * strlen(Messages::epsilonWarningText1)) / 2;
  y += fontSize.height() + 5;
  ctx->drawString(Messages::epsilonWarningText1, KDPoint(initPos, y), KDFont::LargeFont, KDColorWhite, KDColorRed);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::epsilonWarningText2)) / 2;
  y += fontSize.height() + 2;
  ctx->drawString(Messages::epsilonWarningText2, KDPoint(initPos, y), KDFont::SmallFont, KDColorWhite, KDColorRed);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::epsilonWarningText3)) / 2;
  y += KDFont::SmallFont->glyphSize().height() + 5;
  ctx->drawString(Messages::epsilonWarningText3, KDPoint(initPos, y), KDFont::SmallFont, KDColorWhite, KDColorRed);
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::epsilonWarningText4)) / 2;
  y += KDFont::SmallFont->glyphSize().height() + 10;
  ctx->drawString(Messages::epsilonWarningText4, KDPoint(initPos, y), KDFont::SmallFont, KDColorWhite, KDColorRed);
  y += KDFont::SmallFont->glyphSize().height() + 10;
  initPos = (320 - KDFont::SmallFont->glyphSize().width() * strlen(Messages::epsilonWarningText5)) / 2;
  ctx->drawString(Messages::epsilonWarningText5, KDPoint(initPos, y), KDFont::SmallFont, KDColorWhite, KDColorRed);
  y += KDFont::SmallFont->glyphSize().height() + 5;
  ctx->drawString(Messages::epsilonWarningText6, KDPoint(initPos, y), KDFont::SmallFont, KDColorWhite, KDColorRed);
}

}
