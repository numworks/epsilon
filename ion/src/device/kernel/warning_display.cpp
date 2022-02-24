#include "warning_display.h"
#include <drivers/events.h>
#include <drivers/keyboard.h>
#include <drivers/display.h>
#include <ion/display.h>
#include <kandinsky/font.h>
#include <kandinsky/rect.h>
#include <ion/src/shared/keyboard_queue.h>
#include <string.h>

namespace Ion {
namespace Device {
namespace WarningDisplay {

void drawString(const char * text, KDCoordinate * yOffset, const KDFont * font) {
  KDSize glyphSize = font->glyphSize();
  KDFont::RenderPalette palette = font->renderPalette(KDColorBlack, KDColorWhite);
  KDFont::GlyphBuffer glyphBuffer;
  size_t len = strlen(text);
  KDPoint position((Ion::Display::Width - len*glyphSize.width())/2, *yOffset);

  while (*text) {
    font->setGlyphGrayscalesForCharacter(*text++, &glyphBuffer);
    font->colorizeGlyphBuffer(&palette, &glyphBuffer);
    // Push the character on the screen
    Ion::Device::Display::pushRect(KDRect(position, glyphSize), glyphBuffer.colorBuffer());
    position = position.translatedBy(KDPoint(glyphSize.width(), 0));
  }
  *yOffset += glyphSize.height();
}

void showMessage(const char * const * messages, int numberOfMessages) {
  KDRect screen = KDRect(0,0,Ion::Display::Width,Ion::Display::Height);
  Ion::Device::Display::pushRectUniform(screen, KDColorWhite);
  const char * title = messages[0];
  KDCoordinate currentHeight = 60;
  drawString(title, &currentHeight, KDFont::LargeFont);
  currentHeight += KDFont::LargeFont->glyphSize().height();
  for (int j = 1; j < numberOfMessages; j++) {
    const char * message = messages[j];
    // Move draw string here are you're the only on to use it!
    drawString(message, &currentHeight, KDFont::SmallFont);
  }
}

void waitUntilKeyPress() {
  /* Prevent the stalling timer from going off and messing up the display */
  Events::pauseStallingTimer();
  while (Keyboard::scan() != 0) {
    Ion::Timing::msleep(100);
  }
  while (Keyboard::scan() == 0) {
    Ion::Timing::msleep(100);
  }
  /* Flush the keyboard queue to avoid delayed reactions. */
  Keyboard::Queue::sharedQueue()->flush();
}

constexpr static int sUnauthenticatedUserlandNumberOfMessages = 6;
constexpr static const char * sUnauthenticatedUserlandMessages[sUnauthenticatedUserlandNumberOfMessages] = {
  "UNOFFICIAL SOFTWARE",
  "Caution: You are using an unofficial",
  "software. This software does not match",
  "the requirements of some exams.",
  "NumWorks is not responsible for problems",
  "that arise from the use of this software."
};

void unauthenticatedUserland() {
  showMessage(sUnauthenticatedUserlandMessages, sUnauthenticatedUserlandNumberOfMessages);
  waitUntilKeyPress();
}

constexpr static int sKernelUpgradeRequiredNumberOfMessages = 8;
constexpr static const char * sKernelUpgradeRequiredMessages[sKernelUpgradeRequiredNumberOfMessages] = {
  "OFFICIAL UPGRADE REQUIRED",
  "The software you were about",
  "to execute requires a more",
  "recent authentified kernel",
  "to be executed. Please upgrade",
  "your calculator with the last",
  "official software before trying",
  "again."
};

void upgradeRequired() {
  // TODO image instead of words
  showMessage(sKernelUpgradeRequiredMessages, sKernelUpgradeRequiredNumberOfMessages);
  waitUntilKeyPress();
}

constexpr static int sExternalAppsAvailableNumberOfMessages = 6;
constexpr static const char * sExternalAppsAvailableMessages[sExternalAppsAvailableNumberOfMessages] = {
  "THIRD PARTY APPLICATIONS",
  "You have installed external",
  "applications. For security reasons,",
  "the use of the LED has been disabled.",
  "To erase installed apps, press",
  "reset on the back of the device."
};

void externalAppsAvailable() {
  showMessage(sExternalAppsAvailableMessages, sExternalAppsAvailableNumberOfMessages);
  waitUntilKeyPress();
}

}
}
}
